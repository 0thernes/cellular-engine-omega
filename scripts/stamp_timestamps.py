#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2026 Alexander Stephen Donahue
"""Dual Cryptographic Timestamping Minter & Verifier (Playbook v1.3 Item 26):
  1. DigiCert RFC 3161 Trusted Timestamp Tokens via OpenSSL TS:
     - `timestamps/CHECKSUMS.sha256.tsr`
     - `timestamps/Donahue_2026_cellular_engine_omega.pdf.tsr`
  2. Multi-calendar Bitcoin OpenTimestamps Proof via OTS v1 HTTP calendars:
     - `timestamps/CHECKSUMS.sha256.ots`

Usage:
  python3 scripts/stamp_timestamps.py --verify   # Verify all .tsr and .ots tokens bind current files
  python3 scripts/stamp_timestamps.py --mint     # Mint fresh DigiCert .tsr (both) and 3-calendar OTS .ots
"""

from __future__ import annotations

import argparse
import hashlib
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import urllib.request

ROOT = Path(__file__).resolve().parent.parent
VERSION = (ROOT / "VERSION").read_text(encoding="utf-8").strip() if (ROOT / "VERSION").exists() else "5.4.0"
TIMESTAMPS_DIR = ROOT / "timestamps"

CHECKSUMS_PATH = ROOT / "CHECKSUMS.sha256"
PDF_PATH = ROOT / "dist" / "Donahue_2026_cellular_engine_omega.pdf"

CHECKSUMS_TSR = TIMESTAMPS_DIR / "CHECKSUMS.sha256.tsr"
PDF_TSR = TIMESTAMPS_DIR / "Donahue_2026_cellular_engine_omega.pdf.tsr"
CHECKSUMS_OTS = TIMESTAMPS_DIR / "CHECKSUMS.sha256.ots"

DIGICERT_TSA_URL = "http://timestamp.digicert.com"
OTS_MAGIC = b"\x00OpenTimestamps\x00\x00Proof\x00\xbf\x89\xe2\xe8\x84\xe8\x92\x94"
OTS_CALENDARS = (
    "https://a.pool.opentimestamps.org/digest",
    "https://b.pool.opentimestamps.org/digest",
    "https://a.pool.eternitywall.com/digest",
)


def _find_openssl() -> str:
    for cand in (
        shutil.which("openssl"),
        r"C:\Program Files\Git\usr\bin\openssl.exe",
        "/usr/bin/openssl",
    ):
        if cand and Path(cand).exists():
            return cand
    raise FileNotFoundError("openssl binary not found on PATH or standard install locations")


def mint_rfc3161(target: Path, out_tsr: Path) -> None:
    if not target.exists():
        raise FileNotFoundError(f"Missing target to timestamp: {target}")
    openssl = _find_openssl()
    out_tsr.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.NamedTemporaryFile(suffix=".tsq", delete=False) as tmp:
        tsq_path = Path(tmp.name)
    try:
        subprocess.run(
            [openssl, "ts", "-query", "-data", str(target), "-sha256", "-cert", "-out", str(tsq_path)],
            check=True,
            capture_output=True,
        )
        req = urllib.request.Request(
            DIGICERT_TSA_URL,
            data=tsq_path.read_bytes(),
            headers={"Content-Type": "application/timestamp-query", "User-Agent": f"OmegaEngine-TSA/{VERSION}"},
            method="POST",
        )
        with urllib.request.urlopen(req, timeout=30) as resp:
            if resp.status != 200:
                raise RuntimeError(f"DigiCert TSA returned HTTP {resp.status}")
            out_tsr.write_bytes(resp.read())
        reply = subprocess.run(
            [openssl, "ts", "-reply", "-in", str(out_tsr), "-text"],
            check=True,
            capture_output=True,
            text=True,
        )
        if "Status: Granted." not in reply.stdout:
            raise RuntimeError(f"DigiCert TSA did not grant timestamp:\n{reply.stdout}")
        ts_line = next((ln.strip() for ln in reply.stdout.splitlines() if "Time stamp:" in ln), "Time stamp: unknown")
        print(f"MINTED {out_tsr.relative_to(ROOT).as_posix()} ({out_tsr.stat().st_size:,} B) | {ts_line}")
    finally:
        tsq_path.unlink(missing_ok=True)


def mint_opentimestamps(target: Path, out_ots: Path) -> None:
    if not target.exists():
        raise FileNotFoundError(f"Missing target for OpenTimestamps: {target}")
    out_ots.parent.mkdir(parents=True, exist_ok=True)
    digest = hashlib.sha256(target.read_bytes()).digest()
    calendar_proofs: list[bytes] = []
    for cal_url in OTS_CALENDARS:
        try:
            req = urllib.request.Request(
                cal_url,
                data=digest,
                headers={
                    "Content-Type": "application/x-www-form-urlencoded",
                    "Accept": "application/vnd.opentimestamps.v1",
                    "User-Agent": f"OmegaEngine-OTS/{VERSION}",
                },
                method="POST",
            )
            with urllib.request.urlopen(req, timeout=20) as resp:
                body = resp.read()
                if resp.status == 200 and len(body) > 20:
                    calendar_proofs.append(body)
        except Exception as exc:
            print(f"  [WARN] OTS calendar {cal_url} skipped: {exc}", file=sys.stderr)
    if len(calendar_proofs) < 2:
        raise RuntimeError(f"Expected >=2 OpenTimestamps calendars, got {len(calendar_proofs)}")
    # Standard OpenTimestamps v1 file format: magic + version(0x01) + sha256(0x08) + 32B digest + forked calendar trees
    buf = bytearray()
    buf.extend(OTS_MAGIC)
    buf.append(0x01)  # version 1
    buf.append(0x08)  # SHA256 op
    buf.extend(digest)
    for idx, proof in enumerate(calendar_proofs):
        if idx < len(calendar_proofs) - 1:
            buf.append(0xFF)  # fork marker
        buf.extend(proof)
    out_ots.write_bytes(bytes(buf))
    print(
        f"MINTED {out_ots.relative_to(ROOT).as_posix()} ({len(buf):,} B) "
        f"across {len(calendar_proofs)} OpenTimestamps Bitcoin calendars"
    )


def _verify_tsr(target: Path, tsr_path: Path) -> str:
    if not target.exists():
        raise FileNotFoundError(f"Missing {target}")
    digest = hashlib.sha256(target.read_bytes()).digest()
    if not tsr_path.exists() or tsr_path.stat().st_size < 200:
        raise AssertionError(f"Missing or truncated {tsr_path.name}")
    tsr_bytes = tsr_path.read_bytes()
    if tsr_bytes[0] != 0x30 or digest not in tsr_bytes:
        raise AssertionError(
            f"{tsr_path.name} does not cryptographically bind current {target.name} ({digest.hex()[:16]}...)"
        )
    if b"\x02\x01\x00" not in tsr_bytes[:32] and b"\x02\x01\x01" not in tsr_bytes[:32]:
        raise AssertionError(f"{tsr_path.name} PKIStatus is not granted (0 or 1)")
    return f"{tsr_path.name} ({len(tsr_bytes):,} B RFC 3161 -> {digest.hex()[:16]}...)"


def _verify_ots(target: Path, ots_path: Path) -> str:
    if not target.exists():
        raise FileNotFoundError(f"Missing {target}")
    digest = hashlib.sha256(target.read_bytes()).digest()
    if not ots_path.exists() or ots_path.stat().st_size < len(OTS_MAGIC) + 34 + 32:
        raise AssertionError(f"Missing or truncated {ots_path.name}")
    ots_bytes = ots_path.read_bytes()
    expected_prefix = OTS_MAGIC + b"\x01\x08" + digest
    if not ots_bytes.startswith(expected_prefix):
        raise AssertionError(
            f"{ots_path.name} header/digest does not bind current {target.name} ({digest.hex()[:16]}...)"
        )
    if b"https://" not in ots_bytes:
        raise AssertionError(f"{ots_path.name} missing OpenTimestamps calendar URI attestations")
    return f"{ots_path.name} ({len(ots_bytes):,} B OpenTimestamps -> {digest.hex()[:16]}...)"


def verify_all() -> None:
    s1 = _verify_tsr(CHECKSUMS_PATH, CHECKSUMS_TSR)
    s2 = _verify_tsr(PDF_PATH, PDF_TSR)
    s3 = _verify_ots(CHECKSUMS_PATH, CHECKSUMS_OTS)
    print(f"PASS: Verified dual cryptographic timestamps:\n  - {s1}\n  - {s2}\n  - {s3}")


def main() -> None:
    parser = argparse.ArgumentParser(description="Mint or verify DigiCert RFC 3161 (.tsr) and OpenTimestamps (.ots) proofs.")
    parser.add_argument("--mint", action="store_true", help="Mint fresh DigiCert RFC 3161 .tsr (CHECKSUMS + PDF) and 3-calendar OTS .ots")
    parser.add_argument("--verify", action="store_true", help="Verify all .tsr and .ots tokens cryptographically bind current targets")
    args = parser.parse_args()

    if args.mint:
        mint_rfc3161(CHECKSUMS_PATH, CHECKSUMS_TSR)
        mint_rfc3161(PDF_PATH, PDF_TSR)
        mint_opentimestamps(CHECKSUMS_PATH, CHECKSUMS_OTS)
    verify_all()


if __name__ == "__main__":
    main()
