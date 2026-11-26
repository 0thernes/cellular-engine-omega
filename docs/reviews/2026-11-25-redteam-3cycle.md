# 10-Pass Adversarial Red-Team Review — CELLULAR ENGINE OMEGA (`v5.2.0`, `B45`)

**Review Date:** `2026-11-25`  
**Reviewer Protocol:** *Defensible Independent AI-Assisted Research Playbook (v1.3)* Section 9 (10-Pass Review) & *Build Standard for LLMs (v1.2.0)* 10 Finding Classes (`bug`, `error`, `issue`, `problem`, `concern`, `dilemma`, `conflict`, `gap`, `hole`, `blindspot`).  
**Scope:** Complete 3-cycle audit across all C11 sources (`src/`, `include/`, `test/`), Pure Lean 4.35.0 suite (`lean/`), Python oracle & verification ladder (`reference/`, `tests/`, `schema/`), Web Observatory (`web/`), AMS-LaTeX white paper (`paper/`, `dist/`), CI workflows (`.github/workflows/`), and Mega-Atlas claims (`0thernes/unsolved-problems-mega-atlas`).

---

## Pass-by-Pass Red-Team Findings & Verified Resolutions

### Pass 1 — Mathematical & Formal-Proof Rigor (`lean/`)
- **[issue] `F-01` (Unnecessary `Classical.choice` in 17 Lean theorems):** Monolithic `omega` calls on conjunctions and biconditionals pulled `Classical.choice` into 17 theorems in `v5.1.0`.  
  - *Resolution:* Refactored proofs constructively (`exact ⟨by omega, ...⟩`) across all modules. Live audit (`results/lean4-omega-audit.txt`) confirms **`0 / 89` theorems depend on `Classical.choice`** (`17` axiom-free, `10` `[propext]`, `62` `[propext, Quot.sound]`).
- **[hole] `F-02` (No isolated statement-only challenge / non-vacuity checks in `v5.1.0`):** Statements and proofs lived in the same modules with zero concrete non-vacuity witnesses.  
  - *Resolution:* Split pure definitions into proof-free `Spec.lean`, created statement-only `Challenge.lean` and sorry-free `Solution.lean` with `comparator.json` (`permitted_axioms = ["propext", "Quot.sound"]`), and added `17` witnesses & sharp negative-boundary refutations (`NonVacuity.lean`, `O73`–`O89`). Verified locally across `lean`, `leanchecker`, `leanchecker-paranoid`, `con-leche --verified` (`58,614` decls), and `con-ron --verified`, plus `lake comparator --paranoid` in Ubuntu CI.

### Pass 2 — Numerical Stability, Stochastic Variance & Compiler Parity (`src/`, `build*.sh`)
- **[bug] `F-03` (`PM21` 20% multi-seed flake in `src/omega_substrates_cube.c`):** Newly added multi-seed Cube gate `G57` immediately caught `PM21` failing seeds `2026003` and `2026006` because single-trial stochastic rounding had $\sigma = 0.190$ vs tolerance $0.25$ ($1.31\sigma$).  
  - *Resolution:* Replaced with an unbiased 16-trial mean estimator ($\sigma = 0.0475$, $5.26\sigma$ margin). Verified `20 / 20` seeds pass (`max RMSE = 0.0615 << 0.25`) and locked with regression test `test_b07_regression_pm21_multiseed_stochastic_rounding`.
- **[bug] `F-04` (Cross-OS FMA divergence in `build_linux.sh`):** Linux GCC lacked `-ffp-contract=off` (present in `build.ps1`), risking FMA contraction drift.  
  - *Resolution:* Added `-ffp-contract=off` to `build_linux.sh` and verified `-O0` vs `-O3` produces **bit-identical** Master SHA-256 (`a92641843e46736b...`).

### Pass 3 — Statistical Validity & Preregistration Discipline (`PREREGISTRATION.md`, `scripts/`)
- **[error] `F-05` (Mutant-ID mismatch in initial `PREREGISTRATION.md`):** Initial draft mapped `H2` to centrosome centering under `M11` and `H8` to `M16`, whereas `M11` actually disables `Ca2+` MT catastrophe (`mt_after_ca`) and `M10` disables `TB16` projection. Caught immediately on first execution of `run_stats_and_benchmarks.py`.  
  - *Resolution:* Corrected mapping to match compiled gates `G43`/`G44`, logged transparently as `DEV-06` in `DEVIATIONS.md`, kept the exact locked seed range (`2026001`–`2026020`), and verified **`8 / 8` contrasts pass at `p_Holm = 0.0008 < 0.001`**.

### Pass 4 — Input Validation, Security & Fuzzing (`src/omega_main_cli.c`, `web/server.mjs`)
- **[hole] `F-06` (Silent CLI argument acceptance & missing fuzz harness):** `v5.1.0` silently accepted `--seed 0`, `--mutant 99`, and unknown flags, and had no fuzz target.  
  - *Resolution:* Implemented `parse_u64_strict`, `parse_mutant_strict`, `--version`, `--help`, exit code `2` on invalid inputs, plus `test/fuzz_omega_cli_codec.c` (`256/256` mutated cases + `32` `prop_check` seeds PASS) and `test_b06_cli_rejects_invalid_arguments`.
- **[concern] `F-07` (Loopback Web Observatory observability & accessibility):** Missing `/healthz` alias, structured JSON logs, and WCAG 2.2 AA navigation landmarks/skip-link.  
  - *Resolution:* Hardened `web/server.mjs`, `web/index.html`, and `web/styles.css` with `/healthz`, JSON logging, skip-to-content link, `<main>`/`<nav>`, `role="status"`, `:focus-visible`, and `prefers-reduced-motion`, plus STRIDE threat model in `docs/threat-model.md`.

### Pass 5 — Scientific Scope & Mega-Atlas Honesty (`docs/mega-atlas-cross-check.md`)
- **[blindspot] `F-08` (Unchecked claims against 1,900 Mega-Atlas open problems):** Prior version lacked an explicit audit against `0thernes/unsolved-problems-mega-atlas`.  
  - *Resolution:* Audited all 1,900 problems in `MegaAtlas_Unsolved_Problems_V2.json.gz`. Documented honest verdict **`0 / 1,900` solved** with explicit 22-problem scoped mapping explaining why every parent problem remains **OPEN**.

### Pass 6 — Legal Licensing & Author Attribution (`LICENSE`, `LICENSE-DOCS`, `R8`)
- **[problem] `F-09` (Unformed entity `"0thernes LLC"` & non-standard proprietary license):** Prior `v5.1.0` used `"0thernes LLC"` and custom `"Cosmogonic License"`.  
  - *Resolution:* Replaced globally with **`Alexander Stephen Donahue` · `Independent Researcher, Spokane, WA, USA` · ORCID `0009-0006-8168-9648`** and dual open licensing **Apache-2.0 (`LICENSE`) + CC BY 4.0 (`LICENSE-DOCS`)**, guarded by `scripts/verify_version_sync.py`.

### Pass 7 — Documentation, Schema & Drift Synchronization
- **[error] `F-10` (Stale SHA-256 digests, versions & theorem counts in `README.md`, `VERIFICATION.md`, headers):** `v5.1.0` had mixed `v2.0.0`, `v5.0`, `66/66`, and pre-FMA hashes.  
  - *Resolution:* Single version source `VERSION` (`5.2.0`), automated `scripts/verify_version_sync.py`, `scripts/check_readme_axioms.py`, `scripts/generate_checksums.py`, and JSON Schema Draft 2020-12 contracts (`schema/`).
- **[dilemma] `F-11` (`LICENSE` vs `LICENSE-DOCS` filename convention):** Resolved in ADR 0004 by placing Apache-2.0 in `LICENSE` (for clean GitHub SPDX detection of this C11/Lean codebase) and CC BY 4.0 in `LICENSE-DOCS`.

### Pass 8 — White Paper Typography, Citations & Clean-Room Packaging (`paper/`, `dist/`)
- **[error] `F-12` (LaTeX overfull/float warnings, unescaped `#`, `\nolinkurl` space stripping, and unused citation):** First compile surfaced unescaped `#`, a `29.5pt` oversized table float, 2 hyperref PDF-string warnings, 1 unused BibTeX entry, and visual PDF inspection of Pages 2/3/4/9 revealed `\nolinkurl{...}` collapsing spaces inside multi-flag CLI commands.  
  - *Resolution:* Added `\PassOptionsToPackage{obeyspaces}{url}` before `xurl`, fixed all LaTeX formatting, cited all `31/31` BibTeX entries (`unused = 0`, `0` malformed DOIs), verified **0 errors / 0 overfull boxes / 0 float warnings**, visually re-verified pages, and verified deterministic `dist/arxiv_submission.tar.gz` compiles cleanly in an empty temp directory.

### Pass 9 — Platform & Toolchain Conflicts (`B23`, `B25`, Windows `bwrap`)
- **[conflict] `F-13` (Windows `pytest` bracketed path parsing & Windows `lake comparator` `bwrap` requirement):** Passing absolute paths with `[Vibe Coded (AI)]` to `pytest` triggers parametrization syntax error; `lake comparator` requires Linux `bwrap` namespaces.  
  - *Resolution:* Documented relative-path invocation in `docs/configuration.md` and `AGENTS.md`; executed `5` independent Lean checkers locally on Windows (`lean`, `leanchecker`, `leanchecker-paranoid`, `con-leche`, `con-ron`) and wired `lake comparator --config comparator.json --paranoid` + `bubblewrap` in Ubuntu Linux CI.

### Pass 10 — Build Standard v1.2.0 Completeness & Independent Replication
- **[gap] `F-14` (Baseline `Audit-Repo.ps1` score `13.2%` — `46` failing checks):** `v5.1.0` lacked `justfile`, `repo.yaml`, `CLAIMS.md`, `LIMITATIONS.md`, `REPLICATION.md`, `PREREGISTRATION.md`, `CITATION.cff`, ADRs, threat model, social preview, GitHub Pages, Blueprint, Python oracle, Hypothesis tests, and release/checksum workflows.  
  - *Resolution:* Implemented every single missing artifact cleanly with real evidence; verified via `Audit-Repo.ps1` and independent clean-directory replication (`REPLICATION.md`).
