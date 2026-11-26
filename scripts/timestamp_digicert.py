#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2026 Alexander Stephen Donahue
"""RFC 3161 Trusted Timestamping & Verification via DigiCert TSA (zero external dependencies).

Encodes a standard RFC 3161 TimeStampReq (SHA-256 OID 2.16.840.1.101.3.4.2.1, certReq=TRUE),
submits to http://timestamp.digicert.com, writes `<out>.tsr`, and verifies that the DER
TimeStampResp reports PKIStatus=0 (granted) and embeds the exact 32-byte SHA-256 digest
of the target file.

Usage:
  python3 scripts/timestamp_digicert.py CHECKSUMS.sha256 timestamps/CHECKSUMS.sha256.tsr
  python3 scripts/timestamp_digicert.py --verify CHECKSUMS.sha256 timestamps/CHECKSUMS.sha256.tsr
"""

from __future__ import annotations

import hashlib
from pathlib import Path
import secrets
import sys
import urllib.request

TSA_URL = "http://timestamp.digicert.com"

# DER OID 2.16.840.1.101.3.4.2.1 (id-sha256)
OID_SHA256 = bytes([0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01])
DER_NULL = bytes([0x05, 0x00])


def der_len(n: int) -> bytes:
    if n < 0x80:
        return bytes([n])
    b = n.to_bytes((n.bit_length() + 7) // 8, "big")
    return bytes([0x80 | len(b)]) + b


def der_tlv(tag: int, body: bytes) -> bytes:
    return bytes([tag]) + der_len(len(body)) + body


def build_tsq(digest: bytes) -> bytes:
    # AlgorithmIdentifier ::= SEQUENCE { algorithm OID, parameters NULL }
    alg_id = der_tlv(0x30, OID_SHA256 + DER_NULL)
    # MessageImprint ::= SEQUENCE { hashAlgorithm AlgorithmIdentifier, hashedMessage OCTET STRING }
    msg_imprint = der_tlv(0x30, alg_id + der_tlv(0x04, digest))
    # version INTEGER { v1(1) }
    version = der_tlv(0x02, b"\x01")
    # nonce INTEGER (64-bit positive)
    nonce_bytes = b"\x00" + secrets.token_bytes(8)
    nonce = der_tlv(0x02, nonce_bytes.lstrip(b"\x00") or b"\x01")
    if nonce[2] & 0x80:
        nonce = der_tlv(0x02, b"\x00" + nonce[2:])
    # certReq BOOLEAN TRUE
    cert_req = bytes([0x01, 0x01, 0xFF])
    return der_tlv(0x30, version + msg_imprint + nonce + cert_req)


def verify_tsr(target: Path, tsr_path: Path) -> bool:
    if not target.exists() or not tsr_path.exists():
        print(f"FAIL: missing {target} or {tsr_path}", file=sys.stderr)
        return False
    digest = hashlib.sha256(target.read_bytes()).digest()
    tsr = tsr_path.read_bytes()
    # Basic ASN.1 TimeStampResp check: SEQUENCE { PKIStatusInfo (SEQUENCE { INTEGER 0|1 ... }), timeStampToken }
    if len(tsr) < 128 or tsr[0] != 0x30:
        print(f"FAIL: {tsr_path} is not a valid DER TimeStampResp", file=sys.stderr)
        return False
    # Granted PKIStatus = 0x30 0x03 0x02 0x01 0x00 (or grantedWithMods = 0x01)
    if b"\x02\x01\x00" not in tsr[:32] and b"\x02\x01\x01" not in tsr[:32]:
        print(f"FAIL: {tsr_path} PKIStatus is not granted (0 or 1)", file=sys.stderr)
        return False
    if OID_SHA256 not in tsr or (der_tlv(0x04, digest) not in tsr):
        print(f"FAIL: {tsr_path} does not embed exact SHA-256 imprint of {target.name}", file=sys.stderr)
        return False
    print(f"PASS: RFC 3161 TSR {tsr_path.name} ({len(tsr):,} B) verified against SHA-256 of {target.name} ({digest.hex()[:16]}...).")
    return True


def stamp_file(target: Path, out_tsr: Path) -> int:
    digest = hashlib.sha256(target.read_bytes()).digest()
    tsq = build_tsq(digest)
    req = urllib.request.Request(
        TSA_URL,
        data=tsq,
        headers={
            "Content-Type": "application/timestamp-query",
            "User-Agent": "0thernes-RFC3161/5.3.0",
        },
        method="POST",
    )
    with urllib.request.urlopen(req, timeout=20) as resp:
        if resp.getcode() != 200:
            print(f"FAIL: DigiCert TSA returned HTTP {resp.getcode()}", file=sys.stderr)
            return 1
        tsr = resp.read()
    out_tsr.parent.mkdir(parents=True, exist_ok=True)
    out_tsr.write_bytes(tsr)
    return 0 if verify_tsr(target, out_tsr) else 1


def main() -> int:
    args = sys.argv[1:]
    if len(args) == 3 and args[0] == "--verify":
        return 0 if verify_tsr(Path(args[1]), Path(args[2])) else 1
    if len(args) == 2:
        return stamp_file(Path(args[0]), Path(args[1]))
    print("Usage: timestamp_digicert.py [--verify] <target_file> <output.tsr>", file=sys.stderr)
    return 2


if __name__ == "__main__":
    raise SystemExit(main())
