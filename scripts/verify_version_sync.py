#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2026 Alexander Stephen Donahue
"""Verify single-version-source synchronization across all repo files (Build Standard A08)."""

from __future__ import annotations

from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parent.parent


def main() -> int:
    version = (ROOT / "VERSION").read_text(encoding="utf-8").strip()
    assert re.fullmatch(r"\d+\.\d+\.\d+", version), f"Invalid semver in VERSION: {version}"

    checks = [
        ("include/omega_abi.h", rf'#define\s+OMEGA_VERSION_STR\s+"{re.escape(version)}"'),
        ("lean/lakefile.toml", rf'version\s*=\s*"{re.escape(version)}"'),
        ("lean/formalization.yaml", rf'version:\s*"{re.escape(version)}"'),
        ("CITATION.cff", rf'version:\s*"{re.escape(version)}"'),
        (".zenodo.json", rf'"version":\s*"{re.escape(version)}"'),
        ("pyproject.toml", rf'version\s*=\s*"{re.escape(version)}"'),
        ("codemeta.json", rf'"version":\s*"{re.escape(version)}"'),
        ("croissant.json", rf'"version":\s*"{re.escape(version)}"'),
        ("CHANGELOG.md", rf'## \[{re.escape(version)}\]'),
        ("repo.yaml", rf'version:\s*"{re.escape(version)}"'),
    ]

    failures: list[str] = []
    for rel, pattern in checks:
        p = ROOT / rel
        if not p.exists():
            failures.append(f"MISSING: {rel}")
            continue
        txt = p.read_text(encoding="utf-8-sig")
        if not re.search(pattern, txt):
            failures.append(f"MISMATCH in {rel}: expected pattern {pattern}")

    # Also enforce Hard Rule R8: zero unformed '0thernes LLC' strings in active files
    skip_dirs = {".git", ".lake", "bin", "paper/build"}
    for p in ROOT.rglob("*"):
        if not p.is_file():
            continue
        rel = p.relative_to(ROOT).as_posix()
        if any(rel == d or rel.startswith(d + "/") for d in skip_dirs):
            continue
        if p.suffix.lower() in {".pdf", ".png", ".jpg", ".gz", ".tsr", ".exe", ".dll", ".a", ".olean", ".ilean"}:
            continue
        if rel in {"ERRATA.md", "HARDENING_LOG.md", "CHANGELOG.md", "AGENTS.md", "llms-full.txt", "scripts/verify_version_sync.py", "docs/reviews/2026-11-25-redteam-3cycle.md", "docs/reviews/2026-11-26-redteam-v530.md", "docs/adr/0004-dual-license-and-independent-researcher.md"}:
            continue
        try:
            txt = p.read_text(encoding="utf-8")
        except UnicodeDecodeError:
            continue
        if "0thernes LLC" in txt:
            failures.append(f"ATTRIBUTION DRIFT (R8): found '0thernes LLC' in {rel}")

    if failures:
        for f in failures:
            print("FAIL:", f, file=sys.stderr)
        return 1
    print(f"PASS: Version {version} synchronized across all {len(checks)} manifests and zero '0thernes LLC' drift.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
