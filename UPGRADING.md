# Upgrading Guide

## Upgrading from `v5.1.0` to `v5.2.0`

### Breaking & Behavioral Changes
1. **Strict CLI Argument Validation (`bin/omega_cell`):**
   - Previously, unknown flags or invalid `--seed 0` / `--mutant 99` values were silently ignored or coerced.
   - In `v5.2.0`, invalid CLI flags, non-numeric or zero `--seed`, and out-of-range `--mutant` (`0..16`) now print an explicit diagnostic to `stderr` and exit with status code **`2`**.
   - New standard flags: `--version` (prints `5.2.0`) and `--help`.
2. **`PM21` Stochastic Rounding Estimator & Cube SHA-256 (`src/omega_substrates_cube.c`):**
   - `PM21` was upgraded from a 1-trial stochastic rounding estimate (which exhibited a `20%` multi-seed flake on seeds `2026003` and `2026006`) to an unbiased 16-trial estimator (`5.26-sigma` stability margin).
   - As a direct consequence, `cube_sha256` and `receipt_sha256` changed deterministically from `v5.1.0`; `master_sha256` for `TB01`–`TB16` (`a9264184...`) is **unchanged**.
3. **Cross-OS Floating-Point Contraction (`build_linux.sh`):**
   - Linux builds now pass `-ffp-contract=off` to match Win64 Clang (`build.ps1`), eliminating FMA contraction divergence between platforms.
4. **Dual Open Licensing (`LICENSE` + `LICENSE-DOCS`):**
   - Replaced the legacy proprietary non-commercial license with OSI-approved **Apache-2.0** (`LICENSE`) for all source code, Lean proofs, and scripts, and **CC BY 4.0** (`LICENSE-DOCS`) for white papers, figures, datasets, and prose.
5. **Lean 4.35.0 Module Architecture (`lean/`):**
   - Pure definitions moved to `lean/OmegaCellularProofs/Spec.lean` to isolate statement-only `lean/Challenge.lean` from sorry-free `lean/Solution.lean` for `lake comparator --config comparator.json --paranoid`.
   - Theorem count increased from `72` to `89` (adding `O73`–`O89` in `OmegaCellularProofs.NonVacuity`).
   - Standalone axiom audit moved to `lean/VerifyAxioms.lean`.
6. **JSON Schema Addition (`tb04`):**
   - `omega_cell json` now includes `"mass_drift_active"` alongside `"mass_drift_passive"` in `tb04` (additive, backward-compatible for existing consumers).
7. **Web Observatory (`web/server.mjs`):**
   - Added `/healthz` alias alongside `/api/health`; `/api/health` now reports `"version": "5.2.0"`, `"lean_theorems": 89`, `"lean_kernels": 6`, `"gates": 60`.

### Deprecation Policy
- Stable C11 ABI entrypoints in `include/omega_abi.h` follow SemVer (`MAJOR.MINOR.PATCH`). Any future breaking ABI signature change will increment `MAJOR` and include a migration table here.
