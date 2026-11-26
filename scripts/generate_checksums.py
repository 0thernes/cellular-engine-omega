#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2026 Alexander Stephen Donahue
"""Generate and verify CHECKSUMS.sha256 and data/MANIFEST.sha256 (Build Standard B37, B39)."""

from __future__ import annotations

import hashlib
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parent.parent

RELEASE_TARGETS = [
    "VERSION",
    "LICENSE",
    "LICENSE-DOCS",
    "CITATION.cff",
    "croissant.json",
    "ro-crate-metadata.json",
    "codemeta.json",
    "lean/Challenge.lean",
    "lean/Solution.lean",
    "lean/comparator.json",
    "lean/formalization.yaml",
    "lean/OmegaCellularProofs/Spec.lean",
    "lean/OmegaCellularProofs/AdversarialTests.lean",
    "lean/VerifyAxioms.lean",
    "results/lean4-omega-audit.txt",
    "results/multiseed_stats.csv",
    "paper/main.tex",
    "paper/references.bib",
    "dist/Donahue_2026_cellular_engine_omega.pdf",
    "dist/arxiv_submission.tar.gz",
]

DATA_TARGETS = [
    "results/multiseed_stats.csv",
    "results/lean4-omega-audit.txt",
    "lean/VerifyAxioms.lean",
    "schema/omega_run.schema.json",
    "schema/omega_cube.schema.json",
]


def sha256_file(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        for chunk in iter(lambda: f.read(65536), b""):
            h.update(chunk)
    return h.hexdigest()


def write_manifest(out_path: Path, targets: list[str]) -> int:
    out_path.parent.mkdir(parents=True, exist_ok=True)
    lines = []
    for rel in targets:
        p = ROOT / rel
        if p.exists():
            lines.append(f"{sha256_file(p)}  {rel}")
    out_path.write_text("\n".join(lines) + "\n", encoding="utf-8", newline="\n")
    return len(lines)


def verify_manifest(manifest_path: Path) -> bool:
    ok = True
    for raw in manifest_path.read_text(encoding="utf-8").splitlines():
        raw = raw.strip()
        if not raw:
            continue
        expected, rel = raw.split("  ", 1)
        p = ROOT / rel
        if not p.exists():
            print(f"MISSING: {rel}", file=sys.stderr)
            ok = False
            continue
        actual = sha256_file(p)
        if actual != expected:
            print(f"MISMATCH: {rel} expected={expected} actual={actual}", file=sys.stderr)
            ok = False
    return ok


def main() -> int:
    if "--verify" in sys.argv:
        ok1 = verify_manifest(ROOT / "CHECKSUMS.sha256")
        ok2 = verify_manifest(ROOT / "data" / "MANIFEST.sha256")
        if ok1 and ok2:
            print("PASS: All CHECKSUMS.sha256 and data/MANIFEST.sha256 digests verified.")
            return 0
        return 1
    n1 = write_manifest(ROOT / "CHECKSUMS.sha256", RELEASE_TARGETS)
    n2 = write_manifest(ROOT / "data" / "MANIFEST.sha256", DATA_TARGETS)
    print(f"Wrote CHECKSUMS.sha256 ({n1} files) and data/MANIFEST.sha256 ({n2} files).")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
