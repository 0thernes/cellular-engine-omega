#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2026 Alexander Stephen Donahue
"""Validate paper/references.bib cite-key hygiene AND live DOI + Crossref title/author/retraction match.

Playbook v1.3 Pass 5 / Item 16 (hardened in v5.4.0):
- Verifies zero duplicate keys, zero missing keys, zero uncited entries in paper/main.tex,
- Verifies every DOI via https://doi.org/api/handles/ (HTTP 200, responseCode == 1),
- Queries Crossref https://api.crossref.org/works/<doi> for EVERY entry and verifies:
  1. Zero retraction / withdrawal notices,
  2. Resolved Crossref title matches the BibTeX title (token Jaccard + containment check),
  3. Resolved Crossref first author surname (or institutional publisher/author) matches BibTeX author.
- Optionally writes a full machine-readable JSON audit receipt (`--write-receipt results/citation-audit.json`).
"""

from __future__ import annotations

import argparse
from datetime import datetime, timezone
import html
import json
from pathlib import Path
import re
import sys
import time
import unicodedata
import urllib.error
import urllib.parse
import urllib.request

ROOT = Path(__file__).resolve().parent.parent
VERSION = (ROOT / "VERSION").read_text(encoding="utf-8").strip() if (ROOT / "VERSION").exists() else "5.4.0"

STOPWORDS = {
    "a", "an", "and", "as", "at", "by", "for", "from", "in", "into", "is",
    "of", "on", "or", "the", "to", "via", "with", "i", "ii", "iii",
}


def strip_latex_and_accents(text: str) -> str:
    """Convert common BibTeX LaTeX escapes and accents into plain ASCII lowercase."""
    s = html.unescape(text)
    s = re.sub(r"<[^>]+>", " ", s)
    s = re.sub(r"\{\\[\"'`^~=.uvHc]\{?([A-Za-z])\}?\}", r"\1", s)
    s = re.sub(r"\\[\"'`^~=.uvHc]\{?([A-Za-z])\}?", r"\1", s)
    s = s.replace(r"\&", "and").replace("&", "and")
    s = s.replace("---", " ").replace("--", " ").replace("-", " ")
    s = s.replace("{", "").replace("}", "")
    s = unicodedata.normalize("NFKD", s)
    s = "".join(ch for ch in s if not unicodedata.combining(ch))
    return s.lower()


def title_tokens(text: str) -> set[str]:
    clean = strip_latex_and_accents(text)
    toks = re.findall(r"[a-z0-9]+", clean)
    return {t for t in toks if len(t) >= 2 and t not in STOPWORDS}


def extract_bibtex_first_author_surname(author_field: str) -> str:
    clean = strip_latex_and_accents(author_field).strip()
    first = re.split(r"\band\b", clean, maxsplit=1)[0].strip()
    if "," in first:
        surname = first.split(",", 1)[0].strip()
    else:
        parts = [p for p in re.findall(r"[a-z]+", first) if p]
        surname = parts[-1] if parts else ""
    tokens = re.findall(r"[a-z]+", surname)
    return tokens[-1] if tokens else ""


def parse_bibtex_entries(bib_text: str) -> list[dict[str, str]]:
    """Parse top-level @type{key, ...} BibTeX entries preserving brace-nested field values."""
    entries: list[dict[str, str]] = []
    i = 0
    n = len(bib_text)
    while i < n:
        at = bib_text.find("@", i)
        if at == -1:
            break
        m = re.match(r"@(\w+)\s*\{\s*([^,\s]+)\s*,", bib_text[at:])
        if not m:
            i = at + 1
            continue
        entry_type = m.group(1).lower()
        key = m.group(2).strip()
        brace_start = bib_text.find("{", at)
        depth = 0
        j = brace_start
        while j < n:
            if bib_text[j] == "{":
                depth += 1
            elif bib_text[j] == "}":
                depth -= 1
                if depth == 0:
                    break
            j += 1
        body = bib_text[at + m.end():j]
        fields: dict[str, str] = {"type": entry_type, "key": key}
        pos = 0
        bn = len(body)
        while pos < bn:
            fm = re.match(r"\s*([A-Za-z0-9_-]+)\s*=\s*", body[pos:])
            if not fm:
                pos += 1
                continue
            fname = fm.group(1).lower()
            vpos = pos + fm.end()
            if vpos >= bn:
                break
            if body[vpos] == "{":
                fdepth = 0
                k = vpos
                while k < bn:
                    if body[k] == "{":
                        fdepth += 1
                    elif body[k] == "}":
                        fdepth -= 1
                        if fdepth == 0:
                            break
                    k += 1
                val = body[vpos + 1:k].strip()
                pos = k + 1
            elif body[vpos] == '"':
                k = vpos + 1
                while k < bn and body[k] != '"':
                    k += 1
                val = body[vpos + 1:k].strip()
                pos = k + 1
            else:
                k = vpos
                while k < bn and body[k] not in ",\r\n":
                    k += 1
                val = body[vpos:k].strip()
                pos = k + 1
            fields[fname] = re.sub(r"\s+", " ", val)
            while pos < bn and body[pos] in " \t\r\n,":
                pos += 1
        entries.append(fields)
        i = j + 1
    return entries


def check_title_match(bib_title: str, cr_title: str) -> tuple[bool, float]:
    bt = title_tokens(bib_title)
    ct = title_tokens(cr_title)
    if not bt or not ct:
        return False, 0.0
    inter = bt & ct
    overlap_bib = len(inter) / len(bt)
    overlap_cr = len(inter) / len(ct)
    jaccard = len(inter) / len(bt | ct)
    ok = (jaccard >= 0.55) or (overlap_bib >= 0.80 and overlap_cr >= 0.55) or (overlap_cr >= 0.80 and overlap_bib >= 0.55)
    return ok, round(jaccard, 4)


def check_author_match(entry: dict[str, str], cr_msg: dict) -> tuple[bool, str, str]:
    bib_author = entry.get("author", "")
    bib_surname = extract_bibtex_first_author_surname(bib_author)
    cr_authors = cr_msg.get("author") or []
    cr_names: list[str] = []
    for a in cr_authors:
        fam = a.get("family") or a.get("name") or ""
        if fam:
            cr_names.append(fam)
    cr_first = cr_names[0] if cr_names else ""
    cr_first_clean = strip_latex_and_accents(cr_first)

    if bib_surname and cr_first_clean:
        first_tokens = set(re.findall(r"[a-z]+", cr_first_clean))
        if bib_surname in first_tokens or bib_surname in cr_first_clean:
            return True, bib_surname, cr_first

    bib_clean = strip_latex_and_accents(bib_author)
    cr_pub = strip_latex_and_accents(str(cr_msg.get("publisher", "")))
    cr_inst = strip_latex_and_accents(
        " ".join(str(x.get("name", "")) for x in (cr_msg.get("institution") or []) if isinstance(x, dict))
    )
    cr_all_authors = " ".join(strip_latex_and_accents(n) for n in cr_names)
    if entry.get("type") == "techreport" or "institute" in bib_clean or "national" in bib_clean:
        bib_inst_toks = {t for t in re.findall(r"[a-z]+", bib_clean) if len(t) >= 3 and t not in STOPWORDS}
        cr_inst_toks = set(re.findall(r"[a-z]+", f"{cr_pub} {cr_inst} {cr_all_authors}"))
        if bib_inst_toks and len(bib_inst_toks & cr_inst_toks) >= 2:
            return True, bib_clean, cr_msg.get("publisher") or cr_first

    bib_first_part = strip_latex_and_accents(re.split(r"\band\b", bib_author, maxsplit=1)[0])
    bib_first_toks = {t for t in re.findall(r"[a-z]+", bib_first_part) if len(t) >= 3}
    cr_first_toks = {t for t in re.findall(r"[a-z]+", cr_first_clean) if len(t) >= 3}
    if bib_first_toks and cr_first_toks and (bib_first_toks & cr_first_toks):
        return True, bib_surname, cr_first

    return False, bib_surname, cr_first or str(cr_msg.get("publisher", ""))


def _fetch_json_with_retry(req: urllib.request.Request, timeout: int = 20, max_attempts: int = 5) -> tuple[int, dict]:
    """Perform HTTP GET returning (status, json) with polite backoff on HTTP 429 / 5xx."""
    last_exc: Exception | None = None
    for attempt in range(max_attempts):
        try:
            with urllib.request.urlopen(req, timeout=timeout) as resp:
                return resp.getcode(), json.loads(resp.read().decode("utf-8"))
        except urllib.error.HTTPError as exc:
            last_exc = exc
            if exc.code in (429, 500, 502, 503, 504) and attempt + 1 < max_attempts:
                retry_after = exc.headers.get("Retry-After") if exc.headers else None
                try:
                    wait_s = max(1.0, min(15.0, float(retry_after))) if retry_after else (1.5 * (2 ** attempt))
                except ValueError:
                    wait_s = 1.5 * (2 ** attempt)
                time.sleep(wait_s)
                continue
            raise
        except Exception as exc:
            last_exc = exc
            if attempt + 1 < max_attempts:
                time.sleep(1.0 * (attempt + 1))
                continue
            raise
    assert last_exc is not None
    raise last_exc


def verify_entry_live(entry: dict[str, str], timeout: int = 20) -> dict:
    key = entry["key"]
    doi = entry.get("doi", "").strip()
    bib_title = entry.get("title", "")
    record = {
        "key": key,
        "doi": doi,
        "bib_title": bib_title,
        "bib_author": entry.get("author", ""),
        "handle_ok": False,
        "crossref_ok": False,
        "retracted": False,
        "title_match": False,
        "title_jaccard": 0.0,
        "author_match": False,
        "crossref_title": "",
        "crossref_first_author": "",
        "status": "",
        "passed": False,
    }

    api_url = f"https://doi.org/api/handles/{urllib.parse.quote(doi, safe='/')}"
    req = urllib.request.Request(
        api_url,
        headers={"User-Agent": f"0thernes-OmegaCitationVerifier/{VERSION}"},
        method="GET",
    )
    try:
        code, payload = _fetch_json_with_retry(req, timeout=timeout)
        if code != 200:
            record["status"] = f"HANDLE HTTP {code}"
            return record
        if payload.get("responseCode") != 1:
            record["status"] = f"HANDLE code {payload.get('responseCode')}"
            return record
        record["handle_ok"] = True
    except Exception as exc:
        record["status"] = f"HANDLE ERR: {exc}"
        return record

    cr_url = f"https://api.crossref.org/works/{urllib.parse.quote(doi, safe='/')}"
    cr_req = urllib.request.Request(
        cr_url,
        headers={"User-Agent": f"0thernes-OmegaCitationVerifier/{VERSION} (mailto:0_0@0thernes.art)"},
        method="GET",
    )
    try:
        cr_code, cr_payload = _fetch_json_with_retry(cr_req, timeout=timeout)
        if cr_code != 200:
            record["status"] = f"CROSSREF HTTP {cr_code}"
            return record
        msg = cr_payload.get("message", {})
        record["crossref_ok"] = True
    except Exception as exc:
        record["status"] = f"CROSSREF ERR: {exc}"
        return record

    updates = msg.get("update-to", []) or []
    rel = msg.get("relation", {}) or {}
    retracted = any(
        "retract" in str(u.get("type", "")).lower() or "withdraw" in str(u.get("type", "")).lower()
        for u in updates
    ) or ("is-retracted-by" in rel)
    record["retracted"] = retracted
    if retracted:
        record["status"] = "RETRACTED (Crossref)"
        return record

    cr_titles = msg.get("title") or []
    cr_title = " ".join(str(t) for t in cr_titles).strip()
    cr_subtitles = msg.get("subtitle") or []
    cr_full_title = (cr_title + " " + " ".join(str(t) for t in cr_subtitles).strip()).strip() if cr_subtitles else cr_title
    record["crossref_title"] = cr_full_title

    t_ok, jac = check_title_match(bib_title, cr_full_title)
    if not t_ok and cr_title != cr_full_title:
        t_ok2, jac2 = check_title_match(bib_title, cr_title)
        if jac2 > jac:
            t_ok, jac = t_ok2, jac2
    record["title_match"] = t_ok
    record["title_jaccard"] = jac

    a_ok, _, cr_first = check_author_match(entry, msg)
    record["author_match"] = a_ok
    record["crossref_first_author"] = cr_first

    if not t_ok:
        record["status"] = f"TITLE MISMATCH (J={jac:.2f}): CR='{cr_full_title[:70]}'"
        return record
    if not a_ok:
        record["status"] = f"AUTHOR MISMATCH: CR='{cr_first}'"
        return record

    record["passed"] = True
    record["status"] = f"DOI+CR+TITLE+AUTH OK (J={jac:.2f})"
    return record


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Verify paper/references.bib citations, live DOI resolution, Crossref retractions, and title/author match."
    )
    parser.add_argument("--offline", action="store_true", help="Skip network DOI/Crossref checks.")
    parser.add_argument("--allow-unused", action="store_true", help="Do not fail on uncited BibTeX entries.")
    parser.add_argument(
        "--write-receipt",
        type=Path,
        default=None,
        help="Write machine-readable JSON audit receipt (e.g. results/citation-audit.json).",
    )
    args = parser.parse_args()

    bib_path = ROOT / "paper" / "references.bib"
    tex_path = ROOT / "paper" / "main.tex"
    if not bib_path.exists() or not tex_path.exists():
        print("FAIL: paper/main.tex or paper/references.bib missing", file=sys.stderr)
        return 1
    bib = bib_path.read_text(encoding="utf-8")
    tex = tex_path.read_text(encoding="utf-8")

    parsed = parse_bibtex_entries(bib)
    keys = [e["key"] for e in parsed]
    if len(keys) != len(set(keys)):
        print("FAIL: Duplicate BibTeX keys detected in paper/references.bib", file=sys.stderr)
        return 1
    entry_set = set(keys)

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

    bad_dois = []
    for e in parsed:
        d = e.get("doi", "").strip()
        if not d or not re.fullmatch(r"10\.\d{4,9}/\S+", d):
            bad_dois.append((e["key"], d))
    if bad_dois:
        print(f"FAIL: Missing or malformed DOI(s): {bad_dois}", file=sys.stderr)
        return 1

    if args.offline:
        print(
            f"PASS (offline): All {len(cited)} cited keys resolved across {len(parsed)} BibTeX entries "
            f"(unused={len(unused)}, {len(parsed)} syntactically valid DOIs)."
        )
        return 0

    results = []
    failures = []
    for idx, e in enumerate(parsed):
        if idx > 0:
            time.sleep(0.12)
        rec = verify_entry_live(e)
        results.append(rec)
        tag = "OK  " if rec["passed"] else "FAIL"
        print(f"  [{tag}] {rec['key']:<28} {rec['status']:<36} doi:{rec['doi']}")
        if not rec["passed"]:
            failures.append(rec)

    receipt = {
        "schema": f"cellular-engine-omega-citation-audit/v{VERSION}",
        "version": VERSION,
        "audited_utc": datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ"),
        "bib_file": "paper/references.bib",
        "tex_file": "paper/main.tex",
        "cited_keys": len(cited),
        "unused_entries": len(unused),
        "targets_checked": len(results),
        "targets_passed": sum(1 for r in results if r["passed"]),
        "handle_ok_count": sum(1 for r in results if r["handle_ok"]),
        "crossref_ok_count": sum(1 for r in results if r["crossref_ok"]),
        "title_match_count": sum(1 for r in results if r["title_match"]),
        "author_match_count": sum(1 for r in results if r["author_match"]),
        "retractions_found": sum(1 for r in results if r["retracted"]),
        "entries": results,
    }

    if args.write_receipt is not None:
        out_path = args.write_receipt if args.write_receipt.is_absolute() else (ROOT / args.write_receipt)
        out_path.parent.mkdir(parents=True, exist_ok=True)
        out_path.write_text(json.dumps(receipt, indent=2, ensure_ascii=False) + "\n", encoding="utf-8", newline="\n")
        print(f"Wrote citation audit receipt -> {out_path}")

    if failures:
        print(
            f"FAIL: {len(failures)}/{len(results)} reference(s) failed live DOI/Crossref title+author/retraction audit!",
            file=sys.stderr,
        )
        for f in failures:
            print(f"  - {f['key']} ({f['doi']}): {f['status']}", file=sys.stderr)
        return 1

    print(
        f"PASS: All {len(cited)} cited keys resolved ({len(parsed)} entries, unused={len(unused)}) and "
        f"all {receipt['targets_passed']}/{receipt['targets_checked']} DOIs verified live "
        f"(DOI Handle 200, Crossref 0 retractions, Crossref title match {receipt['title_match_count']}/{receipt['targets_checked']}, "
        f"first-author/institution match {receipt['author_match_count']}/{receipt['targets_checked']})."
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
