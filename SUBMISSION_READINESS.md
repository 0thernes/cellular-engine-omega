<!-- SPDX-License-Identifier: CC-BY-4.0 -->
# Submission Readiness Matrix — Defensible Independent AI Research Playbook v1.3

**Repository:** `0thernes/cellular-engine-omega` (`v5.4.0`)  
**Author:** Alexander Stephen Donahue — Independent Researcher, Spokane, WA, USA · ORCID [`0009-0006-8168-9648`](https://orcid.org/0009-0006-8168-9648)  
**Assessment Date:** 2026-11-26  
**Scope Statement:** Independent research preprint monograph and executable C11 + Lean 4.35.0 software-model artifact. **Not peer-reviewed.** Zero (`0 / 1,900`) overarching [`0thernes/unsolved-problems-mega-atlas`](docs/mega-atlas-cross-check.md) (`V2.6.0`) open problems are claimed solved or retired.

---

## 43-Item Gold Standard Checklist (Playbook v1.3)

| # | Playbook v1.3 Requirement | Status | Concrete Evidence / Artifact |
|---:|---|---|---|
| 1 | Dedicated public repository with clean git history on `main` | **DONE** | `https://github.com/0thernes/cellular-engine-omega`, branch `main` (direct commits, zero branch protection) |
| 2 | Dual open license (code + docs/preprint) & REUSE/SPDX | **DONE** | `LICENSE` (Apache-2.0), `LICENSE-DOCS` (CC-BY-4.0), `LICENSES/Apache-2.0.txt`, `LICENSES/CC-BY-4.0.txt`, `REUSE.toml`, SPDX headers |
| 3 | Persistent author identity & ORCID | **DONE** | Alexander Stephen Donahue, Independent Researcher, Spokane, WA, USA · ORCID `0009-0006-8168-9648` (`0` corporate-label drift in active files) |
| 4 | Machine-readable citation metadata | **DONE** | `CITATION.cff` (`v5.4.0`), `CITATION.bib`, `.zenodo.json`, `codemeta.json` |
| 5 | Transparent AI-assistance disclosure (code, proofs, prose) | **DONE** | `docs/AI_DISCLOSURE.md`, `README.md`, `paper/main.tex` (Playbook §4 AI-Contribution Disclosure + CRediT roles), `.zenodo.json` |
| 6 | Explicit Conflict of Interest (COI) & `"not peer-reviewed"` status | **DONE** | Prominent `"not peer-reviewed"` status banner in `README.md` & `paper/main.tex`; explicit COI statement in `paper/main.tex` |
| 7 | Honest claim-boundary & real-vs-metaphor limitations | **DONE** | `LIMITATIONS.md`, `CLAIMS.md` (`C01`–`C10`), `docs/statement-fidelity.md`, `ERRATA.md` (preserving 2026-11-26 statement audit) |
| 8 | Explicit falsification criteria ("what would prove us wrong") | **DONE** | `CLAIMS.md` refutation column + 60 C11 gates (`G01`–`G60`) + 16 compiled structural mutants (`M1`–`M16`) |
| 9 | Zero-sorry, zero-custom-axiom machine-checked Lean 4.35.0 proofs | **DONE** | `lean/` — **100 / 100** zero-Mathlib Pure Lean 4.35.0 discrete software-model lemmas strictly within `{propext, Quot.sound}` (`0` `Classical.choice`, `0` `sorry` outside `Challenge.lean`) |
| 10 | Pinned toolchain & manifest (`lean-toolchain`, `lake-manifest.json`) | **DONE** | `lean/lean-toolchain` (`v4.35.0`), `lean/lake-manifest.json`, zero Mathlib, `autoImplicit = false` |
| 11 | Statement-only Challenge vs Solution separation (Palomar) | **DONE** | `lean/Challenge.lean` (imports ONLY `Spec.lean`) vs `lean/Solution.lean` (`14` proof modules) + `lean/comparator.json` |
| 12 | Multi-checker verification (reference kernel + 4 independent checkers) | **DONE** | Reference kernel (`lean`, `leanchecker`, `leanchecker-paranoid`) + 4 independent checkers (`lean4lean` via `lake check --paranoid`, Rust `nanoda_bin`, `con-leche --verified`, `con-ron --verified`) + `lake comparator --paranoid` |
| 13 | Adversarial false-variant refutations (`¬P`) & non-vacuity witnesses | **DONE** | `lean/OmegaCellularProofs/NonVacuity.lean` (`O73`–`O89`, 17 items) + `AdversarialTests.lean` (`O90`–`O100`, 11 items) |
| 14 | Negative elaboration test + live axiom-gate & comparator tamper tests | **DONE** | `scripts/verify_lean_six_kernels.sh` (negative elaboration test + live `lake comparator --paranoid` `sorry`-tamper rejection) + `scripts/test_lean_negative.py` (`sorry`, custom axiom, `native_decide`, `Classical.choice`) |
| 15 | Statement-fidelity matrix mapping formal defs <-> C11 <-> prose | **DONE** | `docs/statement-fidelity.md` (disclosing all 16 closed literal checks, `O01`/`O56` fixes, and `O63`/`O72` literal-receipt scope) + `lean/formalization.yaml` |
| 16 | Verbatim `#print axioms` output reproduced in `README.md` & CI-diffed | **DONE** | `results/lean4-omega-audit.txt`, `scripts/check_readme_axioms.py`, `diff -u` in CI |
| 17 | Independent exact-arithmetic reference oracle (different language) | **DONE** | `reference/biophysics_oracle.py` (Python `fractions.Fraction`, 50-digit `decimal.Decimal`, `hashlib.sha256`) |
| 18 | Deterministic property-based testing across randomized inputs | **DONE** | `tests/test_omega_verification_ladder.py` (`1,200+` Hypothesis examples) + `test/fuzz_omega_cli_codec.c` (`32` property seeds) |
| 19 | Mutation testing (deliberately broken mutants caught) | **DONE** | `test/test_omega_falsification.c` — `16 / 16` compiled planted mutants (`M1`–`M16`) caught |
| 20 | Metamorphic & conservation invariants verified at runtime | **DONE** | `test_b14_metamorphic_seed_invariance_of_laws`, `G01`–`G60`, exact integer `TB14` FIFO/LIFO/Random confluence |
| 21 | Cross-platform C11 CI & `-O0`/`-O3` `-ffp-contract=off` parity | **DONE** | 3-OS CI (`ubuntu-latest`, `windows-latest`, `macos-latest`); `-O0 == -O3` identical Master SHA-256 on Win64 x86-64 |
| 22 | Numeric edge-value stability & named bug regressions | **DONE** | `G57` / `test_b07_regression_pm21_multiseed_stochastic_rounding` (`PM21` 16-trial fix, `20/20` seeds), `G58` NIST SHA-256 vectors, `G59` edge seeds (`1`, `UINT64_MAX`) |
| 23 | Sanitizers (`ASan`/`UBSan`) & fuzzing harness | **DONE** | `OMEGA_SANITIZE=1 ./build_linux.sh` + `test/fuzz_omega_cli_codec.c` (`256` mutated cases + `LLVMFuzzerTestOneInput`) |
| 24 | JSON Schema Draft 2020-12 contracts & CLI boundary rejection | **DONE** | `schema/omega_run.schema.json`, `schema/omega_cube.schema.json`, `test_b06_cli_rejects_invalid_arguments` (exit code `2`) |
| 25 | Pre-registered quantitative hypotheses & deviation log | **DONE** | `PREREGISTRATION.md` (`H1`–`H8`, seeds `2026001`–`2026020`) + `DEVIATIONS.md` (`DEV-06`) |
| 26 | Non-parametric permutation p-values, 95% CIs, effect sizes, FWER | **DONE** | `scripts/run_stats_and_benchmarks.py` -> `results/multiseed_stats.csv` (`10,000` permutations, bootstrap 95% CIs, Cohen's `dz`, Holm–Bonferroni `p_Holm = 0.0008`) |
| 27 | Live Crossref DOI + title + first-author + 0-retraction audit | **DONE** | `scripts/check_citations.py` + `results/citation-audit.json` (`32 / 32` DOI Handle 200, `0` retractions, `32 / 32` Crossref title `J = 1.00` & author/institution match; Nüchter 2006 DOI fixed to `10.1016/j.cub.2006.03.089`) |
| 28 | Publication-quality AMS-LaTeX preprint PDF (0 overfull, 0 undef refs) | **DONE** | `paper/main.tex` -> `dist/Donahue_2026_cellular_engine_omega.pdf` via Tectonic (also compiled live in CI) |
| 29 | Clean-room deterministic arXiv source tarball (`mtime=0`, USTAR) | **DONE** | `dist/arxiv_submission.tar.gz` verified by clean-room Tectonic rebuild locally & in CI |
| 30 | Interactive static proof blueprint & showcase portal (GitHub Pages) | **DONE** | `blueprint/index.html`, `pages/index.html`, `.github/workflows/pages.yml` |
| 31 | Machine-readable FAIR metadata & Gebru et al. Datasheet | **DONE** | `croissant.json` (Croissant 1.0), `ro-crate-metadata.json` (RO-Crate 1.1), `codemeta.json` (CodeMeta 2.0), `docs/DATASHEET.md` |
| 32 | Cryptographic SHA-256 manifests for all deliverables & data | **DONE** | `CHECKSUMS.sha256` (LF endings), `data/MANIFEST.sha256`, CI `SHA256SUMS` |
| 33 | Dual independent cryptographic timestamps (RFC 3161 + Bitcoin OTS) | **DONE** | `scripts/stamp_timestamps.py` -> `timestamps/CHECKSUMS.sha256.tsr`, `timestamps/Donahue_2026_cellular_engine_omega.pdf.tsr` (DigiCert RFC 3161) + `timestamps/CHECKSUMS.sha256.ots` (3-calendar Bitcoin OpenTimestamps) |
| 34 | SLSA build provenance attestation (Sigstore Rekor) | **DONE** | `actions/attest-build-provenance` in `.github/workflows/ci.yml` & `.github/workflows/release.yml` |
| 35 | Software Heritage (SWH) archival & Master Archive registration | **DONE** | SWH snapshot `swh:1:snp:b1734acc036650d5a32194f3437e93ca83a891b8` + registered as `0TH-BUILD-008` in `0thernes/0thernes-research-archive` |
| 36 | Clean-clone replication protocol & dated replication log | **DONE** | `REPLICATION.md` (`just reproduce`, dated Win64 & Multi-OS CI runs; external human replication invited) |
| 37 | Red-team adversarial audit dossier, errata ledger & hardening log | **DONE** | `docs/reviews/2026-11-25-redteam-3cycle.md`, `docs/reviews/2026-11-26-redteam-v530.md`, `ERRATA.md`, `HARDENING_LOG.md` |
| 38 | Security threat model & report-only static/secret scans | **DONE** | `docs/threat-model.md`, `SECURITY.md`, `.gitleaks.toml`, CI secret scan & `cppcheck` |
| 39 | Loopback Web Observatory CSRF, DNS-rebinding, CSP & WCAG 2.2 AA | **DONE** | `web/server.mjs` (port `8791`), `web/index.html`, `web/styles.css`, CI HTTP security smoke tests |
| 40 | Version single-source-of-truth check across all manifests | **DONE** | `VERSION` (`5.4.0`) + `scripts/verify_version_sync.py` + `Audit-Repo.ps1` (`100.0%`) |
| 41 | Open-problem crosswalk with explicit anti-hype scope discipline | **DONE** | `docs/mega-atlas-cross-check.md` (22 adjacent `V2.6.0` IDs; **`0 / 1,900`** overarching problems retired) |
| 42 | External submission gates (arXiv endorsement / Zenodo / JOSS) | **READY (MANUAL)** | `dist/arxiv_submission.tar.gz` ready pending human arXiv endorsement; `.zenodo.json` ready for human deposit (zero fabricated DOIs) |
| 43 | External independent human replication | **OPEN (INVITED)** | Clean-clone protocol & SHA-256 anchors published; third-party replication openly invited via `REPLICATION.md` and `.github/ISSUE_TEMPLATE/replication.yml` |

---

## Venue Readiness Notes

- **arXiv (`q-bio.QM` / `cs.LO` / `physics.bio-ph`):** Clean-room `dist/arxiv_submission.tar.gz` compiles cleanly under Tectonic with `0` overfull hboxes, `0` undefined references/citations, and all `32/32` BibTeX entries live-verified against Crossref titles, authors, and zero retractions. Requires manual author submission/endorsement.
- **Zenodo Archive:** `.zenodo.json` is synchronized to `v5.4.0`, dual-licensed (`Apache-2.0` / `CC-BY-4.0`), and discloses AI assistance per Zenodo depositor guidelines. No DOI is fabricated prior to deposit.
- **Software Heritage (SWH):** Snapshot SWHID `swh:1:snp:b1734acc036650d5a32194f3437e93ca83a891b8`; interactive Save Code Now URL: `https://archive.softwareheritage.org/save/origin/https://github.com/0thernes/cellular-engine-omega/visit/git/`.
