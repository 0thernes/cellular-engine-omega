# Changelog

All notable changes to **CELLULAR ENGINE OMEGA** are documented in this file.
The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Changed (documentation, 2026-11-26 statement audit)
- Corrected claim wording in `README.md`, `CLAIMS.md` (`C01`, `C05`, `C06`), `LIMITATIONS.md` and `docs/` to match the [2026-11-26 statement audit](https://github.com/0thernes/0thernes-research-archive/blob/main/docs/reviews/2026-11-26-statement-audit.md): the 100 Lean items are discrete `Nat`/`Int`/`List` lemmas about the software model (at least 16 are closed arithmetic checks on literals); the C11-to-Lean receipt is regenerated literals re-checked by `O63`/`O72`, not a cryptographic link; kernel checking is the reference kernel plus four independent checkers; the "negative kernel test" is a negative elaboration test.
- The `5.3.0` entry below says all `32/32` DOIs were verified; the check confirms resolution only, and the DOI for Nüchter et al. (2006) names a different article (correct DOI `10.1016/j.cub.2006.03.089`). Recorded in `ERRATA.md` with the paper and Lean known issues; no code, Lean or paper file changed.

- Future cross-architecture ARM64 (`aarch64`) reference build receipts and CBMC bounded model checking for C11 solvers.

## [5.3.0] - 2026-11-26

### Fixed
- **[error] PRNG documentation misidentification (`AGENTS.md`, `repo.yaml`, `docs/architecture.md`, `docs/adr/0001-bare-crt-c11.md`, `paper/main.tex`):** Corrected all prose references from `"SplitMix64"` to the actual implemented counter-based **`Philox4x32-10`** PRNG (Salmon et al., SC11, `doi:10.1145/2063384.2063405`) in `src/omega_rng_hash.c`.
- **[error] Default Web Observatory port mismatch (`docs/configuration.md`, `docs/operations.md`, `docs/threat-model.md`, `README.md`, `examples/web_api_demo.mjs`):** Unified all documentation and example clients to match `web/server.mjs` default loopback port **`8791`**.
- **[error] Broken relative documentation link (`SUPPORT.md` -> `docs/index.md`):** Created comprehensive `docs/index.md` hub so all internal repository links resolve cleanly (`0` broken links).
- **[error] Stale `PENDING CI RUN` entry in `REPLICATION.md`:** Recorded executed multi-OS GitHub Actions CI verification run (`#45749131672` for `v5.2.0`) alongside `v5.3.0` clean-clone replication receipts.
- **[issue] `omega_cell json` self-describing schema (`src/omega_main_cli.c`, `schema/omega_run.schema.json`):** Added top-level `"version"` and `"master_sha256"` (shared `compute_master_hash` helper guaranteeing byte-exact parity with `omega_cell all`).
- **[issue] Web Observatory structured API request & error logging (`web/server.mjs`):** Added JSON structured logging on `/api/run`, `/api/health`, `/api/lean`, `/api/cube`, `403`, `429`, and `500` subprocess errors (`A34`).
- **[bug] Windows `leanexport` UTF-8 preservation (`build.ps1`):** Used raw stdout redirection so `nanoda_bin` independently verifies all `100` exported constructive theorems locally on Windows (`exit 0`, strict `["propext", "Quot.sound"]` with `unpermitted_axiom_hard_error: true`).

### Added
- **100 / 100 Constructive Pure Lean 4.35.0 Proofs across ALL 6 Independent Kernels (`lean/OmegaCellularProofs/AdversarialTests.lean`, `scripts/verify_lean_six_kernels.sh`):**
  - Added Module 14 (`AdversarialTests.lean`, `O90`–`O100`): 11 constructive adversarial refutations proving weakened, inverted, or off-by-one biophysical variants `False` without `Classical.choice`.
  - Tightened both `build.ps1` and `build_linux.sh` to enforce `>= 100` theorems strictly within `{propext, Quot.sound}` (mechanically banning `Classical.choice`).
  - Added unified 6-kernel verification script `scripts/verify_lean_six_kernels.sh` covering `lean`, `leanchecker`, `leanchecker-paranoid`, `lean4lean` (`lake check --paranoid`), `nanoda_bin`, `con-leche --verified`, `con-ron --verified`, isolated `lake comparator --paranoid`, AND a **live negative Lean 4.35.0 kernel rejection test** (also enforced in `pytest`).
- **Live DOI + Crossref 0-Retraction Citation Checker (`scripts/check_citations.py`):** Verifies all `32/32` BibTeX DOIs (adding Salmon et al. 2011 `Philox4x32-10`) live against DOI Handle API and Crossref retraction metadata, with `--offline` support.
- **FAIR Scholarly Metadata, Datasheet & RFC 3161 Cryptographic Timestamps:**
  - Added `scripts/generate_fair_metadata.py` generating `croissant.json` (MLCommons Croissant 1.0), `ro-crate-metadata.json` (RO-Crate 1.1), and `codemeta.json` (CodeMeta 2.0), plus Gebru et al. `docs/DATASHEET.md`.
  - Added `scripts/timestamp_digicert.py` producing verifiable DigiCert RFC 3161 `.tsr` timestamps under `timestamps/` (`*.tsr binary` in `.gitattributes`).
- **Monograph-Grade AMS-LaTeX White Paper & CI Tectonic Verification (`paper/main.tex`, `.github/workflows/ci.yml`):**
  - Expanded `paper/main.tex` to a complete monograph with governing equations for `TB01`–`TB16`, full 16-row empirical & mutant-knockout ledger, formal `\begin{theorem}` blocks, complete 6-kernel verification matrix, and complete 22-row Mega-Atlas `V2.6.0` cross-check table.
  - Added live Tectonic compilation of both `paper/main.tex` and clean-room `dist/arxiv_submission.tar.gz` directly inside `.github/workflows/ci.yml`.

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
