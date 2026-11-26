# 3-Cycle Forensic Hardening Log — CELLULAR ENGINE OMEGA (`v5.1.0` $\to$ `v5.2.0`)

Structured index and audit ledger of all three complete hardening cycles conducted across every folder, source file, formal proof module, test suite, white paper, and governance manifest against **Build Standard for LLMs `v1.2.0`** and **Defensible Independent AI-Assisted Research Playbook `v1.3`**.

---

## Table of Contents & Audit Scope Index

1. **Cycle 1 — Deep Core, Formal & Scaffold Hardening:**
   - **1A (C11 & Web Core):** `include/`, `src/`, `test/`, `web/`, `build.ps1`, `build_linux.sh`
   - **1B (Constructive Multi-Kernel Lean 4.35.0):** `lean/OmegaCellularProofs/{Spec,LiveC11Receipt,NonVacuity,*}.lean`, `Challenge.lean`, `Solution.lean`, `VerifyAxioms.lean`, `comparator.json`, `formalization.yaml`
   - **1C (Build Standard v1.2.0 Scaffold & Verification Ladder):** `reference/`, `schema/`, `tests/`, `examples/`, `scripts/`, `docs/`, legal & metadata files
2. **Cycle 2 — Empirical Statistics, Figures, AMS-LaTeX White Paper, Blueprint & 10-Pass Red-Team Audit:**
   - Preregistered `N = 20` multi-seed sweep (`scripts/run_stats_and_benchmarks.py`), vector PDF figures (`paper/figures/`), social preview (`docs/media/`), AMS-LaTeX white paper (`paper/main.tex` -> `dist/Donahue_2026_cellular_engine_omega.pdf` + `dist/arxiv_submission.tar.gz`), Lean Proof Blueprint (`blueprint/`), GitHub Pages (`pages/`), and 10-pass adversarial red-team audit (`docs/reviews/2026-11-25-redteam-3cycle.md`)
3. **Cycle 3 — Final Zero-Defect Verification Sweep, Clean-Directory Replication, GitHub CI & Master Archive Registration:**
   - Re-run full C11 + Lean + pytest + doc/checksum audits + `Audit-Repo.ps1` (`100.0%`), clean-directory independent replication (`B44`), GitHub metadata/labels/push to `main` + CI green verification + Software Heritage archival + `0thernes-research-archive` (`0TH-BUILD-008`) registration.

---

## Complete Finding Classification Register (All 10 Build Standard Classes)

| Finding | Class (`R7`) | Cycle | Location | Status |
| :--- | :--- | :---: | :--- | :--- |
| `PM21` 20% multi-seed stochastic rounding flake (`2026003`/`2026006`) | `bug` | 1A | `src/omega_substrates_cube.c` | **FIXED** (16-trial `5.26-sigma`; `20/20` seeds PASS, max RMSE `0.0615`) |
| Missing `-ffp-contract=off` in Linux build allowing FMA divergence | `bug` | 1A | `build_linux.sh` | **FIXED** (`-O0 == -O3` bit-exact Master SHA-256) |
| Stale versions (`v2.0.0`/`v5.0`), stale `66/66` count, stale SHA-256s | `error` | 1A/2 | `README.md`, `VERIFICATION.md`, headers | **FIXED** (`VERSION` single source + `verify_version_sync.py`) |
| Initial `PREREGISTRATION.md` mutant-ID mismatch (`H2`/`H8`) | `error` | 2 | `PREREGISTRATION.md`, `run_stats_and_benchmarks.py` | **FIXED** (aligned to `G44`/`G43`, disclosed in `DEVIATIONS.md` `DEV-06`) |
| LaTeX unescaped `#`, oversized float, unused ref & README wrap regex | `error` | 2 | `paper/main.tex`, `scripts/check_readme_axioms.py` | **FIXED** (`0` LaTeX errors/overfulls, `31/31` cited, `check_readme_axioms` PASS) |
| 17 Lean theorems pulling unnecessary `Classical.choice` | `issue` | 1B | `lean/OmegaCellularProofs/*.lean` | **FIXED** (`0/89` `Classical.choice`, strictly `{propext, Quot.sound}`) |
| Unformed corporate label `"0thernes LLC"` & proprietary license | `problem` | 1C | `LICENSE`, docs, metadata | **FIXED** (`Independent Researcher` + Apache-2.0 / CC BY 4.0) |
| Loopback Web Observatory missing `/healthz`, JSON logs, WCAG 2.2 AA | `concern` | 1A/1C | `web/server.mjs`, `web/index.html`, `web/styles.css` | **FIXED** (verified live + STRIDE `docs/threat-model.md`) |
| Dual-license root file convention (`LICENSE` vs `LICENSE-DOCS`) | `dilemma` | 1C | `LICENSE`, `LICENSE-DOCS`, `ADR 0004` | **DECIDED** (Apache-2.0 in `LICENSE` for SPDX detection + CC BY 4.0 in `LICENSE-DOCS`) |
| Windows `pytest` bracketed-path & `lake comparator` `bwrap` conflict | `conflict` | 1C/2 | `AGENTS.md`, `docs/configuration.md`, `.github/workflows/ci.yml` | **RESOLVED** (relative `pytest` invocation; `5` local Win64 kernels + Ubuntu CI `comparator`) |
| 46 missing Build Standard v1.2.0 files/checks (`13.2%` baseline) | `gap` | 1C/2 | Repo-wide | **CLOSED** (`100.0%` on `Audit-Repo.ps1`) |
| Silent CLI invalid-arg acceptance, no fuzz harness, no `Challenge.lean` | `hole` | 1A/1B | `src/omega_main_cli.c`, `test/fuzz_omega_cli_codec.c`, `lean/` | **CLOSED** (exit code `2`, `256/256` fuzz, isolated `Spec`/`Challenge`/`Solution` + `17` NonVacuity) |
| Unaudited open-problem scope vs `unsolved-problems-mega-atlas` | `blindspot` | 1/2 | `docs/mega-atlas-cross-check.md`, `paper/main.tex` | **CLOSED** (honest `0/1,900` solved + 22 adjacent open problems documented) |
| Wall-clock timing files in `CHECKSUMS.sha256` & `llms-full.txt` R8 quote | `issue` | 3 | `scripts/generate_checksums.py`, `scripts/verify_version_sync.py` | **FIXED** (caught in clean-directory `B44` replication; checksum manifests restricted to strictly deterministic zero-timestamp artifacts) |
