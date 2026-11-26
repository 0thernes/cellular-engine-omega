# Errata

This ledger tracks any substantive errors discovered in **CELLULAR ENGINE OMEGA** code, formal proofs, datasets, or the accompanying technical white paper (`paper/main.tex` / `dist/Donahue_2026_cellular_engine_omega.pdf`) after publication, with permanent attribution and commit links.

## Post-Release Errata (`v5.2.0`, 2026-11-25)

*No post-release errata have been reported for `v5.2.0` yet.*

## Pre-Release Defects Corrected Between `v5.1.0` and `v5.2.0` (Full Disclosure)

| ID | Class | Location | Defect in `v5.1.0` | Resolution in `v5.2.0` |
| :--- | :--- | :--- | :--- | :--- |
| `E-01` | `bug` | `src/omega_substrates_cube.c` (`PM21`) | Single-trial stochastic rounding had `sigma = 0.190` vs `tol = 0.25` (`1.31-sigma`), failing `20%` of seeds (`2026003`, `2026006`) | Replaced with unbiased 16-trial estimator (`sigma = 0.0475`, `5.26-sigma`); locked by regression gate `G57` |
| `E-02` | `bug` | `build_linux.sh` | Missing `-ffp-contract=off` allowed GCC FMA contraction divergence vs Win64 Clang | Added `-ffp-contract=off` to `CFLAGS` |
| `E-03` | `error` | `README.md`, `VERIFICATION.md`, headers | Stale `v2.0.0`/`v5.0` headers, stale `66/66` proof count in boundary prose, stale pre-FMA SHA-256 digests | Synchronized all versions via single source `VERSION` + `scripts/verify_version_sync.py` and re-measured all hashes from live runs |
| `E-04` | `problem` | Docs & metadata | Unformed entity name `"0thernes LLC"` appeared in author/affiliation blocks | Standardized globally to `Alexander Stephen Donahue, Independent Researcher, Spokane, WA, USA` |
| `E-05` | `hole` | `src/omega_main_cli.c` | Unknown CLI flags and invalid `--seed 0` / `--mutant 99` were silently accepted | Added strict parsers returning exit code `2` + negative boundary tests (`B06`) |
| `E-06` | `issue` | `lean/OmegaCellularProofs/*.lean` | `17` theorems pulled unnecessary `Classical.choice` via monolithic `omega` calls on conjunctions/`↔` | Split goals constructively (`exact ⟨by omega, ...⟩`) and isolated `Spec.lean` / `Challenge.lean` / `Solution.lean` |
