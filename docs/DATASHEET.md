# Datasheet for CELLULAR ENGINE OMEGA Datasets (`v5.3.0`)

Structured following Gebru et al. (2021), *"Datasheets for Datasets"* (*Communications of the ACM* 64(12):86–92).

## 1. Motivation
- **Purpose:** Provide transparent, preregistered, bit-exact reproducible empirical datasets (`results/multiseed_stats.csv`, `results/omega_75_substrates_1000_cube.json`, `results/lean4-omega-audit.txt`) substantiating every statistical, numerical, and formal-proof claim in `CLAIMS.md` and `paper/main.tex`.
- **Creator:** Alexander Stephen Donahue (ORCID `0009-0006-8168-9648`, Independent Researcher, Spokane, WA, USA; `0_0@0thernes.art`).
- **Funding:** Self-funded independent research; zero external grants or corporate sponsors.

## 2. Composition
- **`results/multiseed_stats.csv`** (`20` rows × `25` columns): Preregistered paired intact-vs-knockout experimental outputs across deterministic seeds `2026001`–`2026020` for contrasts `H1`–`H8`, Clean-Slate v3.0 75-substrate pass counts, `10×10×10` Hybrid Cube partition counts (`661/274/20/45`), `PM21` RMSE, `TB08` global minimum TUR product, and `TB16` held-out `R²` and `AUROC`.
- **`results/omega_75_substrates_1000_cube.json`**: Seed `2026001` structured JSON output containing all `25` precision codecs (`PM01`–`PM25`), `25` model families (`MF01`–`MF25`), `25` geometry/topology builders (`GW01`–`GW25`), and all `1,000` triples of the Hybrid Cube (`X0Y0Z0`–`X9Y9Z9`).
- **`results/lean4-omega-audit.txt`**: Verbatim `#print axioms` output across all `100` Lean 4.35.0 theorems (`O01`–`O100`), documenting exact constructive dependencies (`0` `Classical.choice`, `0` `sorryAx`, `0` Mathlib).
- **Confidentiality / PII:** Zero human-subject data, zero personal identifiers, zero clinical/wet-lab patient records; 100% synthetic deterministic simulation & formal-proof outputs.

## 3. Collection Process
- Generated deterministically by compiling zero-dependency C11 (`-O3 -std=c11 -Wall -Wextra -Wshadow -Werror -ffp-contract=off`, `Philox4x32-10` PRNG, FIPS 180-4 SHA-256) and running `scripts/run_stats_and_benchmarks.py` and `lean/VerifyAxioms.lean`.
- All 20 seeds (`2026001`–`2026020`) and all 8 confirmatory endpoints (`H1`–`H8`) were locked in `PREREGISTRATION.md` before final sweeps (with mutant-ID typos transparently logged in `DEVIATIONS.md` `DEV-06`). Zero seeds were dropped or cherry-picked.

## 4. Preprocessing, Cleaning & Labeling
- No raw simulation rows were filtered, winsorized, or excluded.
- `PM21` single-trial estimator variance (`20%` flake on `v5.1.0`) was fixed at the C11 source level (`16`-trial unbiased estimator in `v5.2.0+`) rather than by post-hoc filtering (`ERRATA.md` `E-01`).

## 5. Uses & Limitations
- **Recommended Uses:** Independent replication of `cellular-engine-omega`, benchmarking deterministic C11 biophysics and interaction-net engines, verifying multi-kernel Lean 4.35.0 constructive proofs, and validating FAIR/MLCommons Croissant 1.0 pipelines.
- **Prohibited / Out-of-Scope Uses:** Do **not** treat reduced-order simulation telemetry (`TB01`–`TB16`) or the synthetic `JCVI-syn3A`-inspired `TB16` benchmark as wet-lab biological measurements or clinical predictions (`LIMITATIONS.md`).

## 6. Distribution & Maintenance
- **Distribution:** Publicly hosted at `https://github.com/0thernes/cellular-engine-omega`, mirrored to `https://github.com/0thernes/0thernes-research-archive` (`0TH-BUILD-008`), indexed via `croissant.json`, `ro-crate-metadata.json`, `codemeta.json`, and cryptographically sealed by `CHECKSUMS.sha256`, `data/MANIFEST.sha256`, and DigiCert RFC 3161 `.tsr` timestamps (`timestamps/`).
- **License:** CC BY 4.0 (`LICENSE-DOCS`) for datasets, documentation, and figures; Apache-2.0 (`LICENSE`) for C11/Lean/Python generator code.
- **Maintenance & Errata:** Maintained by Alexander Stephen Donahue; post-release corrections are logged permanently in `ERRATA.md`.
