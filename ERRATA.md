# Errata

This ledger tracks any substantive errors discovered in **CELLULAR ENGINE OMEGA** code, formal proofs, datasets, or the accompanying technical white paper (`paper/main.tex` / `dist/Donahue_2026_cellular_engine_omega.pdf`) after publication, with permanent attribution and commit links.

## Post-Release Errata (`v5.3.0`, 2026-11-26)

## 2026-11-26 statement audit

An independent statement audit of commit `4b257af` found the errors below. Wording in `README.md`, `CLAIMS.md`, `LIMITATIONS.md` and the Markdown files under `docs/` was corrected in the same commit as this entry. The frozen paper (`paper/main.tex`, `paper/references.bib`, `dist/Donahue_2026_cellular_engine_omega.pdf`) and the Lean sources are not changed in this pass; their errors are recorded here. Audit log: <https://github.com/0thernes/0thernes-research-archive/blob/main/docs/reviews/2026-11-26-statement-audit.md>.

### Paper and bibliography

| Where | What is wrong | Correction | Affects a theorem statement? |
| :--- | :--- | :--- | :--- |
| `paper/references.bib`, entry `nuchter2006nanosecond` (line 262); cited in `paper/main.tex` lines 208 and 308 | The DOI `10.1016/j.cub.2006.03.079` given for Nüchter et al. (2006), "Nanosecond-scale kinetics of nematocyst discharge", *Current Biology* 16(9):R316–R318, resolves to a different article (Crossref check, 2026-11-26). | The correct DOI is `10.1016/j.cub.2006.03.089`. | No. |
| `paper/main.tex`, Section "Live Citation Audit, FAIR Metadata, RFC 3161 Timestamps, and CRediT" (line 440); the `CHANGELOG.md` `5.3.0` entry ("all `32/32` BibTeX DOIs") | "32/32 OK" counts DOIs that resolve and carry no retraction notice. `scripts/check_citations.py` does not compare the resolved record with the cited title, so the count does not show that each DOI names the cited work; one does not (row above). | Read: "32 DOIs resolve, 0 retraction notices; resolution does not confirm the article, and one DOI (Nüchter et al. 2006) names a different article." | No. |
| `paper/main.tex`, Introduction (line 64), Figure 1 caption (line 76), Section "Strict Constructive Axiom Footprint and Six-Kernel Verification" (line 264 and Table `tab:kernels`) | "All 6 independent Lean 4.35.0 kernels" lists the reference kernel three times (`lean`, `leanchecker`, `leanchecker-paranoid`). Row 9 ("Live Negative Kernel Test") is an elaboration failure of `rfl` on a false literal, not a kernel rejecting a forged proof term. | Read: "the reference kernel (`lean`, `leanchecker`, `leanchecker-paranoid`) and four independent checkers (`lean4lean`, `nanoda_bin`, `con-leche --verified`, `con-ron --verified`)"; call row 9 a negative elaboration test. | No. |
| `paper/main.tex`, Introduction (line 64) and Figure 1 caption (line 76) | "Bind live C11 runtime telemetry into the kernel" suggests a cryptographic or semantic link. `LiveC11Receipt.lean` is a structure of literals written by one C11 run; `O63`/`O72` check equalities among those fields, and no hash is computed in Lean. | Read: "a C11-generated Lean receipt file whose integer and string fields are re-checked by `O63` and `O72` on each build". | No (the prose describing the theorems is narrowed). |
| `paper/main.tex`, Section "Honest Cross-Check Against `unsolved-problems-mega-atlas`" (lines 412, 421) | `O19`, `O20` and `O52` are cited as Poincaré–Hopf defect and Levin bistability results. `O19` checks sums of three fixed integer lists, `O20` that replacing two `+1` charges by one `+2` keeps a list sum, and `O52` compares two hand-built structures. | Read these items as integer list and record checks about the software model, not as the physical results. | No. |

### Other repository files

| Where | What is wrong | Correction | Affects a theorem statement? |
| :--- | :--- | :--- | :--- |
| `scripts/verify_lean_six_kernels.sh` lines 30–42 | The step labelled "live negative Lean 4.35.0 kernel rejection test" fails during elaboration (`rfl` on `661 + 274 + 20 + 45 = 999`). | Call it a negative elaboration test, or add a forged proof term fed to the independent checkers through an export file. | No. |
| `scripts/verify_lean_six_kernels.sh` (git mode `100644`) | The README quickstart ran `./scripts/verify_lean_six_kernels.sh`, which fails on a fresh clone because the file is not executable in git (CI runs `chmod +x` first). | The README now uses `bash scripts/verify_lean_six_kernels.sh`; the file mode can be set with `git update-index --chmod=+x`. | No. |

### Known issues in the formal code (not yet fixed)

| Where | What is wrong | Correction | Affects a theorem statement? |
| :--- | :--- | :--- | :--- |
| `lean/OmegaCellularProofs/TURSpeedLimitsTopo.lean:49` (`O19_poincare_hopf_sphere_defect_impossibility`), `OpenProliferationWeb.lean:34` (`O36_continuous_12axis_morphospace_unbounded_novelty`), `DeepStrataFrontier.lean:93` (`O52_levin_bioelectric_bistable_memory_without_dna_mutation`), `SubstratesCube.lean:17-21` (`O67`–`O69`), `TardigradeVirosphereOmega.lean:16` (`O27`) | Names claim physical results. `O19` is three `decide`-checked sums of fixed `Int` lists; `O36` is `2^12 < 10^12`; `O67`–`O69` are `25 + 25 + 25 = 75`, `661 + 274 + 20 + 45 = 1000` and `10 * 10 * 10 = 1000` by `rfl`; `O27` sums seven literals to `780 ≤ 1000`; `O52` compares two hand-built structures by `simp`. At least 16 of the 100 items are closed checks on literals (bare `decide`/`rfl`: `O07`, `O09`, `O19`, `O23`, `O36`, `O67`, `O68`, `O69`, `O76`, `O92`, `O94`, `O95`, `O100`; literal `let` bindings: `O27`, `O63`, `O72`). | Rename or annotate, for example `O19_defect_list_sum_examples` and `O36_binary_vs_decimal_12axis_count`. A rename changes `comparator.json`, `Challenge.lean`, `VerifyAxioms.lean` and the frozen audit file, so it needs a new release. | Names only; the proved statements are unchanged. |
| `lean/OmegaCellularProofs/PurcellGaugeGeometry.lean:16` (`O01`) | The hypotheses `tFwd > 0` and `tRev > 0` are discarded (`intro _ _`); the content is `pathDisp A [a, b, a] = 0` for antisymmetric `A`. | Drop the unused rate hypotheses, or label them cosmetic. | Yes (the statement would lose two unused hypotheses; it does not become weaker). |
| `lean/OmegaCellularProofs/VirtualCellManifold.lean:45` (`O56`) | The second conjunct `(err ≠ 0 → sumInt xs ≠ 0)` with `err := sumInt xs` is a tautology, closed by `fun hNe => hNe`. | Drop the conjunct or label it cosmetic; the content is `sumScaledShift N (sumInt xs) xs = 0`. | Yes (removes a tautological conjunct). |
| `lean/OmegaCellularProofs/LiveC11Receipt.lean`; `VerifyTraceReceipt.lean:90` (`O63`); `SubstratesCube.lean:34` (`O72`) | The SHA-256 fields are `String` literals. `O63` checks only that the three schedule digests are equal strings; `receiptSha256`, `tb14Sha256`, `tb16Sha256` and `cubeSha256` are not used by any theorem. No hash is computed in Lean. | Describe `O63`/`O72` as equality checks among C-reported literals; do not call the link cryptographic. | No (documentation of the statement). |
| `lean/Solution.lean:8-10`; header comments of `BioInteractionNets`, `CytoskeletonMotorsLLPS`, `DeepStrataFrontier`, `EigenProofreadingInfo`, `NonVacuity`, `OpenProliferationWeb`, `PurcellGaugeGeometry`, `SubstratesCube`, `TardigradeVirosphereOmega`, `TuringFeinbergCRNT`, `TURSpeedLimitsTopo`, `VerifyTraceReceipt`, `VirtualCellManifold`; `Spec.lean:8` | Docstrings say `v5.2.0`; `Solution.lean` says "89 sorry-free ... theorems" and "13 ... module files". The release is `v5.3.0` with 100 theorems in 14 proof modules (`comparator.json`). | Sync the docstrings to `v5.3.0`, 100 theorems, 14 modules. | No (comments only). |

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
