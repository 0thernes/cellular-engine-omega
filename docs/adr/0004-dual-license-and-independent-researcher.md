# ADR 0004: Dual Open Licensing (Apache-2.0 + CC BY 4.0) and Canonical Independent Researcher Attribution

- **Status:** Accepted (`v5.2.0`)
- **Date:** `2026-11-25`

## Context
Prior releases (`<= v5.1.0`) shipped a custom non-commercial license ("Cosmogonic License") and used an unformed entity label (`"0thernes LLC"`), creating legal ambiguity for academic reuse, arXiv/Zenodo compliance, and GitHub SPDX detection.

## Decision
1. Adopt OSI-approved **Apache-2.0** (`LICENSE`) for all source code, headers, Lean 4 proofs, verification scripts, and CI workflows, paired with **Creative Commons Attribution 4.0 International (CC BY 4.0)** (`LICENSE-DOCS`) for white papers, PDFs, diagrams, datasets, and prose documentation.
2. Standardize author and affiliation metadata across all files, LaTeX headers, BibTeX, `CITATION.cff`, `.zenodo.json`, and `formalization.yaml` to **`Alexander Stephen Donahue` · `Independent Researcher, Spokane, WA, USA` · ORCID `0009-0006-8168-9648`**, enforced automatically by `scripts/verify_version_sync.py`.

## Consequences
- **Positive:** Clean GitHub SPDX detection, full compatibility with arXiv, Zenodo, and Software Heritage scholarly standards, and zero unformed corporate entity claims.
- **Negative:** Removes prior non-commercial restriction; commercial reuse is now permitted subject to Apache-2.0 / CC BY 4.0 attribution terms.
