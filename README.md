Zero-dependency C11 cellular-biophysics simulation engine (16 testbeds, 75 computational substrates, 1,000-cell Hybrid Cube) with 100 constructive zero-Mathlib Lean 4.35.0 lemmas that state discrete Nat/Int/List invariants of the engine's software model (several are closed arithmetic checks on hard-coded constants). The C11 output is linked to Lean by a generated receipt file whose integer and string fields `O63` and `O72` re-check on each build.

# CELLULAR ENGINE OMEGA (`v5.3.0`)

[![CI](https://img.shields.io/github/actions/workflow/status/0thernes/cellular-engine-omega/ci.yml?branch=main&label=CI%20Multi-OS%20%2B%206-Kernel%20Lean)](https://github.com/0thernes/cellular-engine-omega/actions/workflows/ci.yml)
[![Lean 4.35.0](https://img.shields.io/badge/Lean%204.35.0-100%2F100%20Constructive-7c3aed)](blueprint/index.html)
[![Falsification](https://img.shields.io/badge/C11%20Falsification-60%2F60%20Gates%20%C2%B7%2016%2F16%20Mutants-059669)](VERIFICATION.md)
[![Substrates & Cube](https://img.shields.io/badge/Substrates%20%2B%20Cube-75%2F75%20%C2%B7%201%2C000%2F1%2C000%20(20%2F20%20Seeds)-0284c7)](results/omega_75_substrates_1000_cube.json)
[![License: Apache-2.0 / CC BY 4.0](https://img.shields.io/badge/License-Apache--2.0%20%2F%20CC--BY--4.0-blue)](LICENSE)
[![ORCID](https://img.shields.io/badge/ORCID-0009--0006--8168--9648-a6ce39)](https://orcid.org/0009-0006-8168-9648)

**Status:** Active (`v5.3.0`, released `2026-11-26`) — actively maintained, clean-clone reproducible, Build Standard v1.2.0 and Playbook v1.3 checklists (self-assessed).<br>
**Author:** Alexander Stephen Donahue · Independent Researcher, Spokane, WA, USA · ORCID [`0009-0006-8168-9648`](https://orcid.org/0009-0006-8168-9648) · `0_0@0thernes.art` · [0thernes.art](https://0thernes.art)  
**White Paper & Blueprint:** [AMS-LaTeX Monograph PDF (`dist/Donahue_2026_cellular_engine_omega.pdf`)](dist/Donahue_2026_cellular_engine_omega.pdf) · [Interactive Lean Proof Blueprint (`blueprint/index.html`)](blueprint/index.html) · [GitHub Pages Showcase](https://0thernes.github.io/cellular-engine-omega/)

![CELLULAR ENGINE OMEGA v5.3.0 Verification Snapshot](docs/media/observatory_snapshot.png)

---

## Quickstart

### Linux / macOS
```bash
git clone https://github.com/0thernes/cellular-engine-omega.git
cd cellular-engine-omega

# Build native C11 (-O3 -Werror -ffp-contract=off), run ASan/UBSan, 60 gates, 16/16 mutants,
# 256 fuzz cases, 75 substrates + 1,000 Hybrid Cube, live C11->Lean export & 100 Lean 4.35.0 proofs
OMEGA_SANITIZE=1 ./build_linux.sh

# Reference kernel + four independent checkers + negative elaboration test + isolated Challenge/Solution comparator
# (the script is not marked executable in git, so call it through bash)
bash scripts/verify_lean_six_kernels.sh

# Independent Python verification ladder (36 tests, 1,200+ Hypothesis property cases, exact oracle)
python3 -m pytest tests/ -v
```

### Windows (PowerShell 7+)
```powershell
git clone https://github.com/0thernes/cellular-engine-omega.git
cd cellular-engine-omega
pwsh -File build.ps1
python -m pytest tests/ -v
pwsh -File scripts/Audit-Repo.ps1
```

---

## Usage & Examples

```bash
# 1. Run all 16 multi-scale cellular biophysics testbeds (deterministic Seed 2026001)
./bin/omega_cell all --seed 2026001

# 2. Emit JSON validated against schema/omega_run.schema.json (or inject compiled mutant M1..M16)
./bin/omega_cell json --seed 2026001
./bin/omega_cell json --seed 2026001 --mutant 8

# 3. Execute Clean-Slate v3.0 75 Substrates + 1,000-Cell Hybrid Cube JSON receipt
./bin/omega_cell cube results/omega_75_substrates_1000_cube.json --seed 2026001

# 4. Export live C11 runtime telemetry into lean/OmegaCellularProofs/LiveC11Receipt.lean
./bin/omega_cell --export-lean-trace lean/OmegaCellularProofs/LiveC11Receipt.lean --seed 2026001

# 5. Launch hardened loopback-only Web Observatory (http://127.0.0.1:8791)
node web/server.mjs
```

Runnable consumer scripts live in [`examples/`](examples/):
- [`examples/quickstart_demo.py`](examples/quickstart_demo.py) — Python JSON API client
- [`examples/c11_abi_example.c`](examples/c11_abi_example.c) — Direct C11 static/shared ABI linkage (`include/omega_abi.h`)
- [`examples/web_api_demo.mjs`](examples/web_api_demo.mjs) — Node.js client for `/api/health`, `/healthz`, and `/api/cube`

---

## Verified Key Metrics (`v5.3.0`, Seed `2026001`)

| Layer | Verified Metric | Evidence Artifact |
| :--- | :--- | :--- |
| **C11 Falsification** | **`60 / 60` gates PASS · `16 / 16` planted mutants caught · `256 / 256` fuzz PASS** | [`results/falsification_win64.txt`](results/falsification_win64.txt) |
| **Optimization Parity** | **`-O0` == `-O3` bit-exact Master SHA-256** (`a92641843e46736b...`, `Philox4x32-10` + FIPS 180-4 SHA-256) | [`docs/benchmarks.md`](docs/benchmarks.md) |
| **75 Substrates & 1,000 Cube** | **`75 / 75` (`25 PM + 25 MF + 25 GW`) · `661 [OK] / 274 [ADAPT] / 20 [OVER] / 45 [EXCL]` · `20 / 20` seeds** | [`results/omega_75_substrates_1000_cube.json`](results/omega_75_substrates_1000_cube.json) |
| **Lean 4.35.0 Proofs** | **`100 / 100` strictly constructive (`24` axiom-free, `10` `[propext]`, `66` `[propext, Quot.sound]`, `0` `Classical.choice`, `0` Mathlib)** | [`results/lean4-omega-audit.txt`](results/lean4-omega-audit.txt) |
| **Kernel Checking** | **Reference kernel (`lean`, `leanchecker`, `leanchecker-paranoid`) plus four independent checkers (`lean4lean` via `lake check --paranoid`, `nanoda_bin`, `con-leche --verified`, `con-ron --verified`); `nanoda_bin`, `con-leche` and `con-ron` replay the export of the 100 `comparator.json` theorems and their dependencies; `lake comparator --paranoid`; a negative elaboration test (a false literal equation must fail). `lean4lean` and the comparator run in Linux CI only.** | [`scripts/verify_lean_six_kernels.sh`](scripts/verify_lean_six_kernels.sh) |
| **Python Ladder & Stats** | **`36 / 36` pytest (`1,200+` Hypothesis cases + live negative Lean test) · `8 / 8` preregistered `N=20` contrasts (`p_Holm = 0.0008 < 0.001`)** | [`results/multiseed_stats.csv`](results/multiseed_stats.csv) |
| **FAIR, Citations & RFC 3161** | **`32` bibliography DOIs resolve through the DOI Handle API with `0` Crossref retraction notices; the check does not confirm that each DOI names the cited article, and the DOI given for Nüchter et al. (2006) names a different one (see [`ERRATA.md`](ERRATA.md)) · Croissant 1.0 + RO-Crate 1.1 + CodeMeta 2.0 · DigiCert RFC 3161 `.tsr`** | [`croissant.json`](croissant.json) |
| **Mega-Atlas Cross-Check** | **`0 / 1,900` overarching open problems retired (22 adjacent parent problems in `V2.6.0` remain strictly OPEN)** | [`docs/mega-atlas-cross-check.md`](docs/mega-atlas-cross-check.md) |

---

## Verbatim Lean 4.35.0 `#print axioms` Excerpt (CI Drift-Checked)

Audited mechanically by [`scripts/check_readme_axioms.py`](scripts/check_readme_axioms.py) against [`results/lean4-omega-audit.txt`](results/lean4-omega-audit.txt) (all 100 theorems strictly within `{propext, Quot.sound}` — zero `Classical.choice`, zero `sorryAx`, zero `native_decide`):

<!-- LEAN_AXIOMS_START -->
```text
'OmegaCellular.PurcellGaugeGeometry.O01_purcell_1dof_reciprocal_cycle_zero' depends on axioms: [propext, Quot.sound]
'OmegaCellular.EigenProofreadingInfo.O06_hopfield_ninio_proofreading_beats_equilibrium' depends on axioms: [propext]
'OmegaCellular.TuringFeinbergCRNT.O12_turing_equal_diffusion_impossibility' depends on axioms: [propext, Quot.sound]
'OmegaCellular.TURSpeedLimitsTopo.O19_poincare_hopf_sphere_defect_impossibility' does not depend on any axioms
'OmegaCellular.BioInteractionNets.O42_active_pair_diamond_strong_confluence' depends on axioms: [propext, Quot.sound]
'OmegaCellular.VirtualCellManifold.O56_virtual_cell_physics_projection_conservation' depends on axioms: [propext, Quot.sound]
'OmegaCellular.VerifyTraceReceipt.O63_live_c11_receipt_verified' depends on axioms: [propext, Quot.sound]
'OmegaCellular.SubstratesCube.O72_live_c11_cube_receipt_verified' does not depend on any axioms
'OmegaCellular.NonVacuity.O74_refute_1dof_nonzero_reciprocal_stroke' depends on axioms: [propext, Quot.sound]
'OmegaCellular.NonVacuity.O89_refute_quantum_cap_11_fits_16kib' does not depend on any axioms
'OmegaCellular.AdversarialTests.O96_refute_bio_inet_mass_leak_on_step' depends on axioms: [propext, Quot.sound]
'OmegaCellular.AdversarialTests.O100_refute_hybrid_cube_partition_drift' does not depend on any axioms
```
<!-- LEAN_AXIOMS_END -->

---

## Honest Verification Boundary & Limitations

- **What IS formally proved in Lean 4.35.0:** Lemmas over `Nat`, `Int` and `List` about the engine's discrete software model (`O01`–`O62`, `O67`–`O71`): commutation of pointwise updates at distinct indices of a `Nat → Int` map, list-sum conservation under local steps, an integer projection that zeroes a list sum, 2×2 integer Jacobian sign algebra, `Nat` power inequalities, and closed arithmetic checks on hard-coded constants. Also 17 non-vacuity and negative-boundary refutations (`O73`–`O89`) and 11 constructive refutations of weakened variants (`O90`–`O100`).
- **Literal checks and theorem names:** At least 16 of the 100 items are closed arithmetic checks on literals, for example `O68`: `661 + 274 + 20 + 45 = 1000`. Several names state the physical motivation, not the statement proved: `O19_poincare_hopf_sphere_defect_impossibility` checks sums of three fixed defect lists, `O36_..._unbounded_novelty` is `2^12 < 10^12`, and `O52_levin_..._without_dna_mutation` compares two hand-built structures. No biological or continuum-physics law (Poincaré–Hopf, Turing instability, TUR) is formalized in general.
- **C11 receipt (`O63`, `O72`):** `lean/OmegaCellularProofs/LiveC11Receipt.lean` is a structure of literals written by one C11 run (`omega_cell --export-lean-trace`, seed `2026001`). `O63` and `O72` check equalities among its integer and string fields, so a changed C11 output that breaks them fails the build. No hash is computed or verified in Lean; the SHA-256 fields are compared only as strings.
- **Scope of the checker runs:** `lean4lean` (`lake check --paranoid`) and `lake comparator` need Linux `bwrap` and run in CI (run `45810663954`), not on Windows. `lean/Challenge.lean` is a statement-only comparator file with 100 intentional `sorry`; the proof modules contain none.
- **What is NOT proved in Lean:** IEEE-754 floating-point PDE convergence (Cahn–Hilliard, Schnakenberg, spherical nematic relaxation) and C11 compiler memory safety — these are verified empirically by 60 gates, 16 compiled structural knockouts, ASan/UBSan, a 256-case fuzz harness, `-O0` vs `-O3` parity, and an exact Python reference oracle.
- **No Overclaimed Breakthroughs:** Cross-checked against all 1,900 problems in [`0thernes/unsolved-problems-mega-atlas`](https://github.com/0thernes/unsolved-problems-mega-atlas) (`V2.6.0`): **`0 / 1,900`** overarching open problems are solved. Full real-vs-metaphor audit in [`LIMITATIONS.md`](LIMITATIONS.md) and [`CLAIMS.md`](CLAIMS.md); corrections from the 2026-11-26 statement audit ([audit log](https://github.com/0thernes/0thernes-research-archive/blob/main/docs/reviews/2026-11-26-statement-audit.md)) are listed in [`ERRATA.md`](ERRATA.md).
- **AI-Assistance Transparency:** AI engineering tools assisted with implementation, tactic exploration, drafting, and red-team auditing under sole human direction and accountability ([`docs/AI_DISCLOSURE.md`](docs/AI_DISCLOSURE.md)).

---

## Documentation Map

- **Core Ledgers:** [`CLAIMS.md`](CLAIMS.md) · [`LIMITATIONS.md`](LIMITATIONS.md) · [`VERIFICATION.md`](VERIFICATION.md) · [`PREREGISTRATION.md`](PREREGISTRATION.md) · [`REPLICATION.md`](REPLICATION.md) · [`HARDENING_LOG.md`](HARDENING_LOG.md) · [`ERRATA.md`](ERRATA.md) · [`DEVIATIONS.md`](DEVIATIONS.md)
- **Technical Reference:** [`docs/index.md`](docs/index.md) · [`docs/architecture.md`](docs/architecture.md) · [`docs/statement-fidelity.md`](docs/statement-fidelity.md) · [`docs/mega-atlas-cross-check.md`](docs/mega-atlas-cross-check.md) · [`docs/DATASHEET.md`](docs/DATASHEET.md) · [`docs/benchmarks.md`](docs/benchmarks.md) · [`docs/threat-model.md`](docs/threat-model.md) · [`docs/operations.md`](docs/operations.md) · [`docs/configuration.md`](docs/configuration.md) · [`docs/faq.md`](docs/faq.md) · [`docs/glossary.md`](docs/glossary.md) · [`docs/adr/README.md`](docs/adr/README.md)
- **White Paper, FAIR & Blueprint:** [`paper/main.tex`](paper/main.tex) · [`dist/Donahue_2026_cellular_engine_omega.pdf`](dist/Donahue_2026_cellular_engine_omega.pdf) · [`blueprint/index.html`](blueprint/index.html) · [`croissant.json`](croissant.json) · [`ro-crate-metadata.json`](ro-crate-metadata.json) · [`codemeta.json`](codemeta.json)

---

## Citation & License

- **Code, Lean 4 Proofs & Scripts:** [Apache License 2.0](LICENSE)
- **White Paper, Figures, Datasets & Prose Docs:** [Creative Commons Attribution 4.0 International (CC BY 4.0)](LICENSE-DOCS)
- **Citation Metadata:** See [`CITATION.cff`](CITATION.cff) and [`CITATION.bib`](CITATION.bib).
