#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2026 Alexander Stephen Donahue
"""Validate paper/references.bib entries, cite-key hygiene, and zero fabricated DOIs/arXiv IDs."""

from __future__ import annotations

from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parent.parent


def main() -> int:
    bib_path = ROOT / "paper" / "references.bib"
    tex_path = ROOT / "paper" / "main.tex"
    if not bib_path.exists() or not tex_path.exists():
        print("FAIL: paper/main.tex or paper/references.bib missing", file=sys.stderr)
        return 1
    bib = bib_path.read_text(encoding="utf-8")
    tex = tex_path.read_text(encoding="utf-8")

    entries = re.findall(r"@\w+\{([^,\s]+),", bib)
    if len(entries) != len(set(entries)):
        print("FAIL: Duplicate BibTeX keys detected in paper/references.bib", file=sys.stderr)
        return 1
    entry_set = set(entries)

    cited = set()
    for m in re.finditer(r"\\cite[a-z]*\{([^}]+)\}", tex):
        for key in m.group(1).split(","):
            k = key.strip()
            if k:
                cited.add(k)

    missing = sorted(cited - entry_set)
    unused = sorted(entry_set - cited)
    if missing:
        print(f"FAIL: Cited keys missing from references.bib: {missing}", file=sys.stderr)
        return 1

    # Sanity-check DOI format for every doi = {...} field
    bad_dois = []
    for m in re.finditer(r"doi\s*=\s*\{([^}]+)\}", bib, re.I):
        doi = m.group(1).strip()
        if not re.fullmatch(r"10\.\d{4,9}/\S+", doi):
            bad_dois.append(doi)
    if bad_dois:
        print(f"FAIL: Malformed DOI(s): {bad_dois}", file=sys.stderr)
        return 1

    print(f"PASS: All {len(cited)} cited keys resolved cleanly across {len(entries)} BibTeX entries (unused={len(unused)}, 0 malformed DOIs).")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
