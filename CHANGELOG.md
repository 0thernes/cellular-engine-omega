# Changelog

All notable changes to **CELLULAR ENGINE OMEGA** are documented in this file.
The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

- Future cross-architecture ARM64 (`aarch64`) reference build receipts and expanded C11/Python differential suites for PDE grid convergence studies.

## [5.2.0] - 2026-11-25

### Fixed
- **[bug] `PM21` multi-seed variance flake (`src/omega_substrates_cube.c`):** Caught by new multi-seed Cube gate `G57` on seeds `2026003` and `2026006` (`20%` failure rate under 1-trial stochastic rounding where `sigma = 0.190` vs tolerance `0.25`, `1.31-sigma`). Replaced with an unbiased 16-trial stochastic rounding estimator (`sigma = 0.0475`, `5.26-sigma` margin); `100%` pass rate across all tested seeds.
- **[bug] Cross-OS FMA divergence (`build_linux.sh`):** Added `-ffp-contract=off` to Linux GCC/Clang flags to match Win64 Clang (`build.ps1`), eliminating fused multiply-add contraction drift across OS runners.
- **[error] Version, hash & attribution drift:** Unified all C11 headers/banners (`OMEGA_VERSION_STR` via single source `VERSION`), Web Observatory `/api/health`, Lean `lakefile.toml`, README SHA-256 digests, and documentation to `v5.2.0` under canonical `Alexander Stephen Donahue (Independent Researcher, Spokane, WA, USA)` attribution (removing unformed `"0thernes LLC"` strings).
- **[hole] Silent CLI argument acceptance (`src/omega_main_cli.c`):** Added strict `parse_u64_strict` / `parse_mutant_strict`, `--version`, `--help`, explicit exit code `2` on unknown flags or invalid seeds/mutant IDs, and `"mass_drift_active"` in `tb04` JSON output.

### Added
- **Constructive 6-Kernel Lean 4.35.0 Formal Suite (`89` theorems, zero Mathlib):**
  - Isolated pure definition module `lean/OmegaCellularProofs/Spec.lean`, statement-only `lean/Challenge.lean`, sorry-free `lean/Solution.lean`, `lean/comparator.json`, `lean/formalization.yaml`, and standalone `lean/VerifyAxioms.lean`.
  - Added `17` non-vacuity inhabitants & sharp negative-boundary refutations (`O73`–`O89` in `lean/OmegaCellularProofs/NonVacuity.lean`).
  - Verified with `lake comparator --config comparator.json --paranoid` and independent kernels (`lean`, `leanchecker`, `leanchecker-paranoid`, `lean4lean`, `nanoda_bin`, `con-leche --verified`, `con-ron --verified`).
- **Expanded C11 Falsification & Fuzzing (`60` gates + `256`-case fuzz harness):**
  - Added `G55` (75 substrates), `G56` (1,000-cell Hybrid Cube), `G57` (5-seed Cube stability sweep), `G58` (NIST FIPS 180-4 SHA-256 vectors), `G59` (edge seeds & extreme DNA payloads), and `G60` (reentrancy & coupled-vs-standalone `TB16` parity).
  - Added `test/fuzz_omega_cli_codec.c` (`LLVMFuzzerTestOneInput` + standalone 256-case harness + 32-seed `prop_check`).
- **Python Verification Ladder & Exact Oracle (`reference/`, `tests/`, `schema/`, `scripts/`):**
  - Independent exact `Fraction`/`Decimal`/integer oracle (`reference/biophysics_oracle.py`), JSON Schema Draft 2020-12 contracts (`schema/omega_run.schema.json`, `schema/omega_cube.schema.json`), `pytest` + `hypothesis` (`1,000+` property examples) + differential + metamorphic + multi-seed statistical tests (exact permutation tests, 10,000-resample bootstrap 95% CIs, Holm–Bonferroni correction) + TB16 held-out ML leakage audit.
- **Publication-Grade AMS-LaTeX White Paper, Vector Figures & Lean Blueprint:**
  - `paper/main.tex` (`amsart`) + `paper/references.bib` compiled cleanly with Tectonic to `dist/Donahue_2026_cellular_engine_omega.pdf` plus clean-room `dist/arxiv_submission.tar.gz`.
  - Interactive Lean Proof Blueprint (`blueprint/index.html`) and GitHub Pages showcase (`pages/index.html`).
- **Build Standard v1.2.0 & Mega-Atlas Cross-Check:**
  - Full dual licensing (`LICENSE` Apache-2.0 + `LICENSE-DOCS` CC BY 4.0), `CLAIMS.md`, `LIMITATIONS.md`, `REPLICATION.md`, `HARDENING_LOG.md`, `PREREGISTRATION.md`, `ERRATA.md`, `DEVIATIONS.md`, `justfile`, `Makefile`, `repo.yaml`, `llms.txt`, `llms-full.txt`, ADRs, STRIDE threat model, WCAG 2.2 AA Web Observatory upgrades, and honest `0/1,900` Mega-Atlas cross-check (`docs/mega-atlas-cross-check.md`).

## [5.1.0] - 2026-11-23

### Added
- Clean-Slate v3.0 75 Substrates (`PM01`–`PM25`, `MF01`–`MF25`, `GW01`–`GW25`) and `10×10×10` 1,000-Cell Hybrid Cube (`661 [OK]`, `274 [ADAPT]`, `20 [OVER]`, `45 [EXCL]`) in `src/omega_substrates_cube.c`.
- Lean Module 12 (`SubstratesCube.lean`, `O67`–`O72`) raising verified theorem count to `72` (`O01`–`O72`).
- Web Observatory `/api/cube` endpoint.

## [5.0.0] - 2026-11-21

### Added
- `TB15` (Deep-Strata Frontier: CISS Magnetite, *Oxytricha* pointer-chain, Cohesin TADs, Dicke superradiance, Levin bioelectric bistability, Cnidocyte >5.38M g) and `TB16` (Virtual-Cell AI Manifold, O56 hyperplane projection, JCVI-syn3A 493/92 dark proteome, 6-tier closure).
- Live C11-to-Lean runtime receipt `O63_live_c11_receipt_verified` (`LiveC11Receipt.lean`).
- `54` falsification gates and `16/16` compiled planted mutants.

## [4.0.0] - 2026-11-20

### Added
- `TB13` (Open-Proliferation 12-Axis Morphospace & 5-Repo Closure) and `TB14` (Lafont Bio-Interaction Nets "Bio-Frostbite" VM with FIFO/LIFO/Random Strong Confluence & Cryptobiotic Bioglass Tun).
- Hardened loopback Web Observatory (`web/server.mjs`).

## [1.0.0] - 2026-11-18

### Added
- Initial zero-dependency C11 biophysics suite (`TB01`–`TB12`) and Pure Lean 4 formalizations (`O01`–`O32`).
