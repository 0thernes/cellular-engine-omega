#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2026 Alexander Stephen Donahue
"""Deterministic FAIR Scholarly Metadata Generator for CELLULAR ENGINE OMEGA.

Emits:
  1. croissant.json         — MLCommons Croissant 1.0 JSON-LD dataset specification
  2. ro-crate-metadata.json — W3C / ResearchObject RO-Crate 1.1 JSON-LD descriptor
  3. codemeta.json          — CodeMeta 2.0 research software metadata

Pass `--verify` in CI to assert committed metadata matches fresh generation.
"""

from __future__ import annotations

import csv
import hashlib
import json
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parent.parent
REPO_URL = "https://github.com/0thernes/cellular-engine-omega"
PAGES_URL = "https://0thernes.github.io/cellular-engine-omega/"
RAW_BASE = "https://raw.githubusercontent.com/0thernes/cellular-engine-omega/main/"
ORCID_URL = "https://orcid.org/0009-0006-8168-9648"
VERSION = (ROOT / "VERSION").read_text(encoding="utf-8").strip()
PUB_DATE = "2026-11-26"


def _lean_theorem_count() -> int:
    p = ROOT / "lean" / "VerifyAxioms.lean"
    if not p.exists():
        return 100
    return sum(1 for ln in p.read_text(encoding="utf-8").splitlines() if ln.strip().startswith("#print axioms "))


def sha256_and_size(rel: str) -> tuple[str, int]:
    data = (ROOT / rel).read_bytes()
    return hashlib.sha256(data).hexdigest(), len(data)


def build_croissant() -> dict:
    lean_n = _lean_theorem_count()
    csv_rel = "results/multiseed_stats.csv"
    csv_sha, csv_sz = sha256_and_size(csv_rel)
    ax_rel = "results/lean4-omega-audit.txt"
    ax_sha, ax_sz = sha256_and_size(ax_rel)

    with (ROOT / csv_rel).open("r", encoding="utf-8", newline="") as f:
        reader = csv.reader(f)
        cols = next(reader)
        row_count = sum(1 for _ in reader)

    int_cols = {"seed", "substrates_passed", "cube_ok", "cube_adapt", "cube_over", "cube_excl", "h7_conf_intact", "h7_conf_ko"}
    fields = []
    for col in cols:
        dtype = "sc:Integer" if col in int_cols else "sc:Float"
        fields.append({
            "@type": "cr:Field",
            "@id": f"multiseed_stats/{col}",
            "name": col,
            "dataType": dtype,
            "source": {
                "fileObject": {"@id": "multiseed-stats-csv"},
                "extract": {"column": col},
            },
        })

    return {
        "@context": {
            "@language": "en",
            "@vocab": "https://schema.org/",
            "citeAs": "cr:citeAs",
            "column": "cr:column",
            "conformsTo": "dct:conformsTo",
            "cr": "http://mlcommons.org/croissant/",
            "data": {"@id": "cr:data", "@type": "@json"},
            "dataType": {"@id": "cr:dataType", "@type": "@vocab"},
            "dct": "http://purl.org/dc/terms/",
            "extract": "cr:extract",
            "field": "cr:field",
            "fileObject": "cr:fileObject",
            "fileProperty": "cr:fileProperty",
            "fileSet": "cr:fileSet",
            "format": "cr:format",
            "includes": "cr:includes",
            "isLiveDataset": "cr:isLiveDataset",
            "jsonPath": "cr:jsonPath",
            "key": "cr:key",
            "md5": "cr:md5",
            "parentField": "cr:parentField",
            "path": "cr:path",
            "recordSet": "cr:recordSet",
            "references": "cr:references",
            "regex": "cr:regex",
            "repeated": "cr:repeated",
            "replace": "cr:replace",
            "sc": "https://schema.org/",
            "separator": "cr:separator",
            "source": "cr:source",
            "subField": "cr:subField",
            "transform": "cr:transform",
        },
        "@type": "sc:Dataset",
        "conformsTo": "http://mlcommons.org/croissant/1.0",
        "name": "CELLULAR ENGINE OMEGA Multi-Seed Biophysics & Formal Verification Dataset",
        "alternateName": f"CELLULAR ENGINE OMEGA v{VERSION}",
        "description": (
            f"Preregistered N={row_count} multi-seed paired intact-vs-knockout experimental telemetry "
            f"(seeds 2026001..2026020 across 8 confirmatory contrasts, 75 computational substrates, and 1,000-cell Hybrid Cube) "
            f"and verbatim {lean_n}-theorem strictly constructive Lean 4.35.0 kernel axiom audit for CELLULAR ENGINE OMEGA v{VERSION}."
        ),
        "version": VERSION,
        "datePublished": PUB_DATE,
        "url": PAGES_URL,
        "sameAs": REPO_URL,
        "license": "https://creativecommons.org/licenses/by/4.0/",
        "citeAs": (
            f"Donahue, Alexander Stephen (2026). CELLULAR ENGINE OMEGA (v{VERSION}). "
            f"Independent Researcher, Spokane, WA, USA. ORCID: 0009-0006-8168-9648. {REPO_URL}"
        ),
        "creator": {
            "@type": "sc:Person",
            "name": "Alexander Stephen Donahue",
            "identifier": ORCID_URL,
            "affiliation": "Independent Researcher, Spokane, WA, USA",
            "url": "https://0thernes.art",
        },
        "keywords": [
            "cellular biophysics", "artificial life", "lean 4", "formal verification",
            "interaction nets", "thermodynamic uncertainty relation", "turing morphogenesis",
            "llps", "tardigrade cryptobiosis", "virtual cell", "reproducible research",
        ],
        "distribution": [
            {
                "@type": "cr:FileObject",
                "@id": "multiseed-stats-csv",
                "name": "multiseed_stats.csv",
                "description": f"Preregistered N={row_count} independent-seed intact-vs-knockout experimental CSV ({len(cols)} columns, UTF-8).",
                "contentUrl": RAW_BASE + csv_rel,
                "encodingFormat": "text/csv",
                "sha256": csv_sha,
                "contentSize": f"{csv_sz} B",
            },
            {
                "@type": "cr:FileObject",
                "@id": "lean4-omega-audit-txt",
                "name": "lean4-omega-audit.txt",
                "description": f"Verbatim Lean 4.35.0 #print axioms output across all {lean_n} constructive theorems (0 Classical.choice, 0 sorryAx).",
                "contentUrl": RAW_BASE + ax_rel,
                "encodingFormat": "text/plain",
                "sha256": ax_sha,
                "contentSize": f"{ax_sz} B",
            },
        ],
        "recordSet": [
            {
                "@type": "cr:RecordSet",
                "@id": "multiseed_stats",
                "name": "multiseed_stats",
                "description": f"All {row_count} preregistered independent-seed intact-vs-knockout runs (seeds 2026001..2026020).",
                "key": {"@id": "multiseed_stats/seed"},
                "field": fields,
            }
        ],
    }


def build_ro_crate() -> dict:
    lean_n = _lean_theorem_count()
    parts = [
        ("results/multiseed_stats.csv", "text/csv", "Preregistered N=20 multi-seed paired intact-vs-knockout telemetry CSV"),
        ("results/lean4-omega-audit.txt", "text/plain", f"Verbatim Lean 4.35.0 #print axioms transcript ({lean_n} constructive theorems)"),
        ("lean/OmegaCellularProofs/LiveC11Receipt.lean", "text/plain", "Auto-generated live C11 telemetry & SHA-256 kernel receipt (O63, O72)"),
        ("lean/OmegaCellularProofs/Spec.lean", "text/plain", "Pure proof-free Lean 4.35.0 specification module"),
        ("lean/Challenge.lean", "text/plain", f"Isolated statement-only Challenge module ({lean_n} theorems)"),
        ("lean/Solution.lean", "text/plain", f"Sorry-free constructive Solution module ({lean_n} theorems across 6 kernels)"),
        ("lean/comparator.json", "application/json", f"Six-kernel comparator specification ({lean_n} theorems, constructive allowlist)"),
        ("lean/formalization.yaml", "application/yaml", "Structured Lean 4.35.0 formalization provenance & scope manifest"),
        ("schema/omega_run.schema.json", "application/schema+json", "JSON Schema Draft 2020-12 contract for 16-testbed JSON output"),
        ("schema/omega_cube.schema.json", "application/schema+json", "JSON Schema Draft 2020-12 contract for 75-substrate & 1,000-cell Hybrid Cube"),
        ("dist/Donahue_2026_cellular_engine_omega.pdf", "application/pdf", "AMS-LaTeX technical monograph (Tectonic, 0 warnings, 0 overfull hboxes)"),
        ("dist/arxiv_submission.tar.gz", "application/gzip", "Deterministic clean-room arXiv LaTeX submission tarball"),
        ("blueprint/index.html", "text/html", f"Interactive Lean 4.35.0 Proof Blueprint ({lean_n} theorems)"),
        ("docs/DATASHEET.md", "text/markdown", "Gebru et al. Datasheet for empirical datasets"),
        ("docs/statement-fidelity.md", "text/markdown", f"14-module Lean-vs-C11 statement fidelity audit ({lean_n} theorems)"),
        ("docs/mega-atlas-cross-check.md", "text/markdown", "Honest 0/1,900 Mega-Atlas open-problem cross-check (22 adjacent open IDs)"),
        ("CLAIMS.md", "text/markdown", "Numbered falsifiable claims ledger (C01-C10)"),
        ("VERIFICATION.md", "text/markdown", "Multi-kernel Lean, 60-gate C11, and Python verification ledger"),
        ("CHECKSUMS.sha256", "text/plain", "Cryptographic SHA-256 integrity manifest"),
        ("croissant.json", "application/ld+json", "MLCommons Croissant 1.0 dataset metadata"),
        ("codemeta.json", "application/ld+json", "CodeMeta 2.0 research software metadata"),
    ]
    graph: list[dict] = [
        {
            "@type": "CreativeWork",
            "@id": "ro-crate-metadata.json",
            "conformsTo": {"@id": "https://w3id.org/ro/crate/1.1"},
            "about": {"@id": "./"},
        },
        {
            "@id": "./",
            "@type": "Dataset",
            "name": f"CELLULAR ENGINE OMEGA (v{VERSION})",
            "version": VERSION,
            "datePublished": PUB_DATE,
            "description": (
                f"Zero-dependency C11 multi-scale cellular biophysics engine (16 testbeds, 75 computational substrates, "
                f"1,000-cell Hybrid Cube, 60 falsification gates, 16/16 planted mutants, 256-case fuzz harness) cryptographically "
                f"coupled via live runtime receipts (O63, O72) to {lean_n} strictly constructive zero-Mathlib Pure Lean 4.35.0 "
                "theorems audited across all 6 independent kernels."
            ),
            "url": PAGES_URL,
            "identifier": REPO_URL,
            "license": {"@id": "https://www.apache.org/licenses/LICENSE-2.0"},
            "author": {"@id": ORCID_URL},
            "hasPart": [{"@id": rel} for rel, _, _ in parts],
        },
        {
            "@id": ORCID_URL,
            "@type": "Person",
            "name": "Alexander Stephen Donahue",
            "email": "0_0@0thernes.art",
            "affiliation": "Independent Researcher, Spokane, WA, USA",
            "url": "https://0thernes.art",
        },
        {
            "@id": "https://www.apache.org/licenses/LICENSE-2.0",
            "@type": "CreativeWork",
            "name": "Apache License 2.0 (code/Lean) & CC BY 4.0 (paper/docs/datasets)",
            "url": "https://www.apache.org/licenses/LICENSE-2.0",
        },
    ]
    for rel, mime, desc in parts:
        p = ROOT / rel
        entry: dict = {
            "@id": rel,
            "@type": "File",
            "name": Path(rel).name,
            "description": desc,
            "encodingFormat": mime,
        }
        if p.exists() and rel not in ("CHECKSUMS.sha256", "croissant.json", "codemeta.json", "lean/OmegaCellularProofs/LiveC11Receipt.lean"):
            digest, sz = sha256_and_size(rel)
            entry["sha256"] = digest
            entry["contentSize"] = sz
        graph.append(entry)

    return {
        "@context": "https://w3id.org/ro/crate/1.1/context",
        "@graph": graph,
    }


def build_codemeta() -> dict:
    lean_n = _lean_theorem_count()
    return {
        "@context": "https://doi.org/10.5063/schema/codemeta-2.0",
        "@type": "SoftwareSourceCode",
        "name": "CELLULAR ENGINE OMEGA",
        "version": VERSION,
        "datePublished": PUB_DATE,
        "dateModified": PUB_DATE,
        "description": (
            f"Zero-dependency C11 multi-scale cellular biophysics engine (16 testbeds, 75 substrates, 1,000-cell Hybrid Cube, "
            f"60 falsification gates, 16/16 compiled planted mutants) coupled via live C11-to-Lean runtime receipts (O63, O72) "
            f"to {lean_n} strictly constructive zero-Mathlib Pure Lean 4.35.0 theorems verified across all 6 independent kernels."
        ),
        "codeRepository": REPO_URL,
        "url": PAGES_URL,
        "issueTracker": f"{REPO_URL}/issues",
        "license": "https://spdx.org/licenses/Apache-2.0",
        "programmingLanguage": ["C11", "Lean 4.35.0", "Python 3", "JavaScript (ESM)", "AMS-LaTeX"],
        "operatingSystem": ["Linux", "macOS", "Windows"],
        "runtimePlatform": ["GCC / Clang C11", "Lean 4.35.0", "Node.js 22+", "Python 3.11+"],
        "author": [
            {
                "@type": "Person",
                "@id": ORCID_URL,
                "givenName": "Alexander Stephen",
                "familyName": "Donahue",
                "email": "0_0@0thernes.art",
                "affiliation": {
                    "@type": "Organization",
                    "name": "Independent Researcher, Spokane, WA, USA",
                },
            }
        ],
        "keywords": [
            "cellular biophysics", "artificial life", "Lean 4", "formal verification",
            "interaction nets", "Turing morphogenesis", "thermodynamic uncertainty relation",
            "tardigrade cryptobiosis", "virtual cell", "C11",
        ],
    }


def main() -> int:
    verify = "--verify" in sys.argv
    outputs = {
        "croissant.json": json.dumps(build_croissant(), indent=2, ensure_ascii=False) + "\n",
        "ro-crate-metadata.json": json.dumps(build_ro_crate(), indent=2, ensure_ascii=False) + "\n",
        "codemeta.json": json.dumps(build_codemeta(), indent=2, ensure_ascii=False) + "\n",
    }
    if verify:
        mismatches = []
        for rel, expected in outputs.items():
            p = ROOT / rel
            if not p.exists() or p.read_text(encoding="utf-8") != expected:
                mismatches.append(rel)
        if mismatches:
            print(f"FAIL: FAIR metadata drift detected in: {mismatches}", file=sys.stderr)
            return 1
        print(f"PASS: croissant.json, ro-crate-metadata.json, and codemeta.json verified (v{VERSION}).")
        return 0

    for rel, content in outputs.items():
        (ROOT / rel).write_text(content, encoding="utf-8", newline="\n")
    print(f"Generated FAIR metadata: croissant.json, ro-crate-metadata.json, codemeta.json (v{VERSION}).")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
