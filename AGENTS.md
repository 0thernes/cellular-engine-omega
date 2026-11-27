# AGENTS.md — CELLULAR ENGINE OMEGA (`v5.4.0`)

Canonical engineering and audit instructions for AI coding agents and human contributors working in `0thernes/cellular-engine-omega` (`Build Standard for LLMs v1.2.0`).

- **Author:** Alexander Stephen Donahue · ORCID `0009-0006-8168-9648` · Independent Researcher, Spokane, WA, USA (`0_0@0thernes.art` · `https://0thernes.art`)
- **Single Version Source:** [`VERSION`](VERSION) (`5.4.0`)
- **Dual License:** Apache-2.0 (`LICENSE`, code/Lean/scripts) + CC BY 4.0 (`LICENSE-DOCS`, white paper/prose/figures/datasets) + `REUSE.toml` / `LICENSES/`

## Hard Rules (`R1`–`R8`)

- **R1 (No gates or locks):** Never add branch protection, required status checks, commit/tag signing, git hooks, Husky, pre-commit, or push-blocking workflows.
- **R2 (Straight to `main`):** Commit and push directly to `main`; do not open PRs or feature branches.
- **R3 (Informational CI):** All GitHub Actions workflows report status only and never block pushes.
- **R4 (No ceremony clutter):** Do not add Dependabot, Renovate, PR templates, CodeRabbit, `.github/copilot-instructions.md`, or `FUNDING.yml`.
- **R5 (Verify before claiming):** Run every test, kernel, compiler, and audit command after your last edit and cite real outputs.
- **R6 (Zero fabrication):** Never invent SHA-256 hashes, theorem counts, benchmarks, p-values, DOIs, citations, or Mega-Atlas resolutions. Every number must come from an actual run of the current code.
- **R7 (10 finding classes):** Classify every finding into exactly one of: `bug`, `error`, `issue`, `problem`, `concern`, `dilemma`, `conflict`, `gap`, `hole`, `blindspot`.
- **R8 (Honest attribution):** Always attribute to `Alexander Stephen Donahue, Independent Researcher, Spokane, WA, USA` (ORCID `0009-0006-8168-9648`); never use `"0thernes LLC"`.

## Architecture & Source Layout

- `include/` & `src/`: Zero-dependency C11 biophysics engine (`TB01`–`TB16`) + Clean-Slate v3.0 75 Substrates (`PM01`–`PM25`, `MF01`–`MF25`, `GW01`–`GW25`) + `10×10×10` 1,000-Cell Hybrid Cube (`omega_substrates_cube.c`) + FIPS 180-4 SHA-256 & counter-based `Philox4x32-10` PRNG (`omega_rng_hash.c`).
- `test/`: `test_omega_falsification.c` (`60` gates `G01`–`G60`, `16/16` planted mutants, 10-seed sweep) + `fuzz_omega_cli_codec.c` (`LLVMFuzzerTestOneInput` + 256-case standalone harness + 32-seed `prop_check`).
- `lean/`: Zero-Mathlib Pure Lean 4.35.0 (`module` system, `autoImplicit = false`) exporting **100 strictly constructive theorems** (`O01`–`O100`), split into pure proof-free `OmegaCellularProofs/Spec.lean`, live C11 receipt `LiveC11Receipt.lean`, 14 proof modules (including `NonVacuity.lean` and `AdversarialTests.lean`), isolated statement-only `Challenge.lean`, sorry-free `Solution.lean`, `comparator.json`, `formalization.yaml`, and standalone `VerifyAxioms.lean`.
- `reference/` & `tests/`: Independent Python `Fraction`/`Decimal` reference oracle (`reference/biophysics_oracle.py`), JSON Schema Draft 2020-12 contracts (`schema/`), and `pytest` + `hypothesis` verification ladder (`tests/test_omega_verification_ladder.py`).
- `web/`: Hardened loopback-only (`127.0.0.1`) Node.js Web Observatory (`server.mjs`, WCAG 2.2 AA `index.html`, `styles.css`, `app.js`).
- `paper/` & `dist/`: Publication-grade AMS-LaTeX white paper (`paper/main.tex`, `paper/references.bib`, vector PDF figures in `paper/figures/`), frozen PDF (`dist/Donahue_2026_cellular_engine_omega.pdf`), and clean-room `dist/arxiv_submission.tar.gz`.
- `blueprint/` & `pages/`: Interactive Lean 4.35.0 Proof Blueprint and GitHub Pages showcase.

## Key Commands

- **Windows full C11 + Lean build & verify:** `pwsh -File build.ps1`
- **Linux full C11 + ASan/UBSan + Lean build & verify:** `OMEGA_SANITIZE=1 ./build_linux.sh`
- **6-kernel Lean comparator:** `cd lean && lake comparator --config comparator.json --paranoid`
- **Python verification ladder:** `pytest tests/ -v`
- **Stats, benchmarks & figures:** `python3 scripts/run_stats_and_benchmarks.py && python3 scripts/generate_figures.py`
- **White paper build:** `cd paper && tectonic -X compile main.tex --outdir build --keep-logs --keep-intermediates`
- **Build Standard v1.2.0 audit:** `pwsh -File scripts/Audit-Repo.ps1` (Windows) or `./scripts/audit-repo.sh` (Linux/macOS)

## Known Traps

1. **PowerShell bracketed paths:** On the Windows ReFS Dev Drive (`Z:\[Vibe Coded (AI)]\ANTIGRAVITY\cellular-engine-omega`), always pass `-LiteralPath` to PowerShell file cmdlets because `[...]` is wildcard syntax.
2. **Cross-OS floating-point determinism:** Keep `-ffp-contract=off` in both `build.ps1` and `build_linux.sh` to prevent FMA contraction divergence between Clang and GCC.
3. **Never touch `LiveC11Receipt.lean` manually:** Always regenerate via `omega_cell --export-lean-trace lean/OmegaCellularProofs/LiveC11Receipt.lean --seed 2026001` so C11-reported literals stay synchronized with `O63`/`O72` equality checks (note: no hash is computed inside Lean).
4. **Honest scope:** `cellular-engine-omega` solves **`0 / 1,900`** overarching Mega-Atlas open problems (`docs/mega-atlas-cross-check.md`); Lean 4 formalizes exact `Nat`/`Int` algebraic invariants and runtime receipts, not IEEE-754 PDE discretization bounds.
