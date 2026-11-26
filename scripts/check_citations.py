#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2026 Alexander Stephen Donahue
"""Validate paper/references.bib cite-key hygiene AND live DOI + Crossref retraction status.

Playbook v1.3 Pass 5 / Item 16:
- Verifies zero duplicate keys, zero missing keys, zero uncited entries in paper/main.tex,
- Verifies every DOI via https://doi.org/api/handles/ AND Crossref retraction metadata
  (unless --offline is passed).
"""

from __future__ import annotations

import argparse
import json
from pathlib import Path
import re
import sys
import urllib.parse
import urllib.request

ROOT = Path(__file__).resolve().parent.parent
VERSION = (ROOT / "VERSION").read_text(encoding="utf-8").strip() if (ROOT / "VERSION").exists() else "5.3.0"


def check_doi_live(doi: str, timeout: int = 15) -> tuple[bool, str]:
    clean_doi = doi.strip()
    api_url = f"https://doi.org/api/handles/{urllib.parse.quote(clean_doi, safe='/')}"
    req = urllib.request.Request(
        api_url,
        headers={"User-Agent": f"0thernes-OmegaCitationVerifier/{VERSION}"},
        method="GET",
    )
    try:
        with urllib.request.urlopen(req, timeout=timeout) as resp:
            if resp.getcode() != 200:
                return False, f"HTTP {resp.getcode()}"
            payload = json.loads(resp.read().decode("utf-8"))
            if payload.get("responseCode") != 1:
                return False, f"DOI code {payload.get('responseCode')}"
    except Exception as exc:
        return False, str(exc)

    if not clean_doi.lower().startswith("10.5281/zenodo."):
        cr_url = f"https://api.crossref.org/works/{urllib.parse.quote(clean_doi, safe='/')}"
        cr_req = urllib.request.Request(
            cr_url,
            headers={"User-Agent": f"0thernes-OmegaCitationVerifier/{VERSION} (mailto:0_0@0thernes.art)"},
            method="GET",
        )
        try:
            with urllib.request.urlopen(cr_req, timeout=timeout) as cr_resp:
                if cr_resp.getcode() == 200:
                    msg = json.loads(cr_resp.read().decode("utf-8")).get("message", {})
                    updates = msg.get("update-to", []) or []
                    rel = msg.get("relation", {}) or {}
                    retracted = any(
                        "retract" in str(u.get("type", "")).lower()
                        or "withdraw" in str(u.get("type", "")).lower()
                        for u in updates
                    ) or ("is-retracted-by" in rel)
                    if retracted:
                        return False, "RETRACTED (Crossref)"
                    return True, "DOI+CR OK"
        except Exception:
            return True, "DOI OK (200)"
    return True, "DOI OK (200)"


def main() -> int:
    parser = argparse.ArgumentParser(description="Verify paper/references.bib citations and live DOI/Crossref status.")
    parser.add_argument("--offline", action="store_true", help="Skip network DOI/Crossref checks.")
    parser.add_argument("--allow-unused", action="store_true", help="Do not fail on uncited BibTeX entries.")
    args = parser.parse_args()

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

    cited: set[str] = set()
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
    if unused and not args.allow_unused:
        print(f"FAIL: Uncited BibTeX entries in references.bib: {unused}", file=sys.stderr)
        return 1

    dois = list(dict.fromkeys(m.group(1).strip() for m in re.finditer(r"doi\s*=\s*\{([^}]+)\}", bib, re.I)))
    bad_dois = [d for d in dois if not re.fullmatch(r"10\.\d{4,9}/\S+", d)]
    if bad_dois:
        print(f"FAIL: Malformed DOI(s): {bad_dois}", file=sys.stderr)
        return 1

    if args.offline:
        print(
            f"PASS (offline): All {len(cited)} cited keys resolved across {len(entries)} BibTeX entries "
            f"(unused={len(unused)}, {len(dois)} syntactically valid DOIs)."
        )
        return 0

    failures = []
    cr_checked = 0
    for d in dois:
        ok, status = check_doi_live(d)
        if status == "DOI+CR OK":
            cr_checked += 1
        print(f"  [{'OK  ' if ok else 'FAIL'}] {status:<14} doi:{d}")
        if not ok:
            failures.append((d, status))
    if failures:
        print(f"FAIL: {len(failures)}/{len(dois)} DOI(s) failed live/retraction check!", file=sys.stderr)
        return 1

    print(
        f"PASS: All {len(cited)} cited keys resolved ({len(entries)} entries, unused={len(unused)}) and "
        f"all {len(dois)}/{len(dois)} DOIs verified live ({cr_checked} Crossref 0-retraction confirmed)."
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
