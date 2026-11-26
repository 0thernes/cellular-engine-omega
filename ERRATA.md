# Errata

This ledger tracks any substantive errors discovered in **CELLULAR ENGINE OMEGA** code, formal proofs, datasets, or the accompanying technical white paper (`paper/main.tex` / `dist/Donahue_2026_cellular_engine_omega.pdf`) after publication, with permanent attribution and commit links.

## Post-Release Errata (`v5.3.0`, 2026-11-26)

*No post-release errata have been reported for `v5.3.0` yet.*

## Defects Discovered in `v5.2.0` and Resolved in `v5.3.0` (Full Disclosure)

| ID | Class | Location | Defect in `v5.2.0` | Resolution in `v5.3.0` |
| :--- | :--- | :--- | :--- | :--- |
| `E-07` | `error` | `AGENTS.md`, `repo.yaml`, `docs/architecture.md`, `docs/adr/0001-bare-crt-c11.md`, `paper/main.tex` | Documentation mislabeled the C11 PRNG in `src/omega_rng_hash.c` as `"SplitMix64"` | Corrected all prose and citations to **`Philox4x32-10`** (Salmon et al., SC11, `doi:10.1145/2063384.2063405`) |
| `E-08` | `error` | `docs/{configuration,operations,threat-model}.md`, `README.md`, `examples/web_api_demo.mjs` | Documented Web Observatory default port as `8787` while `web/server.mjs` binds `8791` | Unified all docs and examples to port **`8791`** |
| `E-09` | `error` | `SUPPORT.md`, `REPLICATION.md` | `SUPPORT.md` linked to non-existent `docs/index.md`; `REPLICATION.md` retained `PENDING CI RUN` stub | Created `docs/index.md` hub (`0` broken links across repo) and recorded verified CI run `#45749131672` |
| `E-10` | `hole` | `.github/workflows/ci.yml` | CI omitted live Tectonic white paper build, `nanoda_bin`, `lake check --paranoid` (`lean4lean`), and live negative Lean kernel test | Wired unified `scripts/verify_lean_six_kernels.sh` + Tectonic compilation of `paper/main.tex` and clean-room `dist/arxiv_submission.tar.gz` into CI |

## Pre-Release Defects Corrected Between `v5.1.0` and `v5.2.0` (Full Disclosure)

| ID | Class | Location | Defect in `v5.1.0` | Resolution in `v5.2.0` |
| :--- | :--- | :--- | :--- | :--- |
| `E-01` | `bug` | `src/omega_substrates_cube.c` (`PM21`) | Single-trial stochastic rounding had `sigma = 0.190` vs `tol = 0.25` (`1.31-sigma`), failing `20%` of seeds (`2026003`, `2026006`) | Replaced with unbiased 16-trial estimator (`sigma = 0.0475`, `5.26-sigma`); locked by regression gate `G57` |
| `E-02` | `bug` | `build_linux.sh` | Missing `-ffp-contract=off` allowed GCC FMA contraction divergence vs Win64 Clang | Added `-ffp-contract=off` to `CFLAGS` |
| `E-03` | `error` | `README.md`, `VERIFICATION.md`, headers | Stale `v2.0.0`/`v5.0` headers, stale `66/66` proof count in boundary prose, stale pre-FMA SHA-256 digests | Synchronized all versions via single source `VERSION` + `scripts/verify_version_sync.py` and re-measured all hashes from live runs |
| `E-04` | `problem` | Docs & metadata | Unformed entity name `"0thernes LLC"` appeared in author/affiliation blocks | Standardized globally to `Alexander Stephen Donahue, Independent Researcher, Spokane, WA, USA` |
| `E-05` | `hole` | `src/omega_main_cli.c` | Unknown CLI flags and invalid `--seed 0` / `--mutant 99` were silently accepted | Added strict parsers returning exit code `2` + negative boundary tests (`B06`) |
| `E-06` | `issue` | `lean/OmegaCellularProofs/*.lean` | `17` theorems pulled unnecessary `Classical.choice` via monolithic `omega` calls on conjunctions/`↔` | Split goals constructively (`exact ⟨by omega, ...⟩`) and isolated `Spec.lean` / `Challenge.lean` / `Solution.lean` |
