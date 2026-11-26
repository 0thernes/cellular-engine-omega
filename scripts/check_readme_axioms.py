#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2026 Alexander Stephen Donahue
"""Drift check verifying that README.md's verbatim #print axioms block matches results/lean4-omega-audit.txt."""

from __future__ import annotations

from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parent.parent


def extract_live_axioms() -> list[str]:
    audit = (ROOT / "results" / "lean4-omega-audit.txt").read_text(encoding="utf-8-sig")
    rx = re.compile(r"^'(OmegaCellular\.[^']+)' (depends on axioms:\s*\[[^\]]+\]|does not depend on any axioms)$", re.M)
    lines = [re.sub(r"\s+", " ", m.group(0)).strip() for m in rx.finditer(audit)]
    if len(lines) != 89:
        raise RuntimeError(f"Expected 89 axiom lines in results/lean4-omega-audit.txt, found {len(lines)}")
    for line in lines:
        if any(bad in line for bad in ("sorryAx", "Lean.ofReduceBool", "Lean.trustCompiler", "_native", "Classical.choice")):
            raise RuntimeError(f"Disallowed axiom in audit output: {line}")
    return lines


def main() -> int:
    live = extract_live_axioms()
    readme = (ROOT / "README.md").read_text(encoding="utf-8")
    m = re.search(r"<!-- LEAN_AXIOMS_START -->(.*?)<!-- LEAN_AXIOMS_END -->", readme, re.S)
    if not m:
        print("FAIL: README.md missing <!-- LEAN_AXIOMS_START --> ... <!-- LEAN_AXIOMS_END --> markers", file=sys.stderr)
        return 1
    block = m.group(1)
    # Every headline line inside the README verbatim block MUST appear verbatim in live audit output
    quoted = re.findall(r"^'(OmegaCellular\.[^']+)' (?:depends on axioms:\s*\[[^\]]+\]|does not depend on any axioms)$", block, re.M)
    if len(quoted) < 8:
        print(f"FAIL: README.md axiom block has too few verbatim lines ({len(quoted)})", file=sys.stderr)
        return 1
    live_set = set(live)
    for line in re.finditer(r"^'(OmegaCellular\.[^']+)' (?:depends on axioms:\s*\[[^\]]+\]|does not depend on any axioms)$", block, re.M):
        if line.group(0).strip() not in live_set:
            print(f"FAIL: README axiom line drifted from live audit: {line.group(0)}", file=sys.stderr)
            return 1
    print(f"PASS: README.md verbatim #print axioms block matches live 89-theorem constructive audit (0 Classical.choice, 0 sorryAx).")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
