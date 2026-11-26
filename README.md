Zero-dependency C11 multi-scale cellular biophysics engine (16 quantitative testbeds, 75 computational substrates, 1,000-cell Hybrid Cube) cryptographically linked via live runtime receipts to 89 constructive zero-Mathlib Pure Lean 4.35.0 theorems audited across independent kernels.

# CELLULAR ENGINE OMEGA (`v5.2.0`)

[![CI](https://img.shields.io/github/actions/workflow/status/0thernes/cellular-engine-omega/ci.yml?branch=main&label=CI%20Multi-OS%20%2B%206-Kernel%20Lean)](https://github.com/0thernes/cellular-engine-omega/actions/workflows/ci.yml)
[![Lean 4.35.0](https://img.shields.io/badge/Lean%204.35.0-89%2F89%20Constructive%20(0%20Mathlib)-7c3aed)](blueprint/index.html)
[![Falsification](https://img.shields.io/badge/C11%20Falsification-60%2F60%20Gates%20%C2%B7%2016%2F16%20Mutants-059669)](VERIFICATION.md)
[![Substrates & Cube](https://img.shields.io/badge/Substrates%20%2B%20Cube-75%2F75%20%C2%B7%201%2C000%2F1%2C000%20(20%2F20%20Seeds)-0284c7)](results/omega_75_substrates_1000_cube.json)
[![License: Apache-2.0 / CC BY 4.0](https://img.shields.io/badge/License-Apache--2.0%20%2F%20CC--BY--4.0-blue)](LICENSE)
[![ORCID](https://img.shields.io/badge/ORCID-0009--0006--8168--9648-a6ce39)](https://orcid.org/0009-0006-8168-9648)

**Status:** Active (`v5.2.0`, released `2026-11-25`) — actively maintained, clean-clone reproducible, Build Standard v1.2.0 compliant.  
**Author:** Alexander Stephen Donahue · Independent Researcher, Spokane, WA, USA · ORCID [`0009-0006-8168-9648`](https://orcid.org/0009-0006-8168-9648) · `0_0@0thernes.art` · [0thernes.art](https://0thernes.art)  
**White Paper & Blueprint:** [AMS-LaTeX PDF (`dist/Donahue_2026_cellular_engine_omega.pdf`)](dist/Donahue_2026_cellular_engine_omega.pdf) · [Interactive Lean Proof Blueprint (`blueprint/index.html`)](blueprint/index.html) · [GitHub Pages Showcase](https://0thernes.github.io/cellular-engine-omega/)

![CELLULAR ENGINE OMEGA v5.2.0 Verification Snapshot](docs/media/observatory_snapshot.png)

---

## Quickstart

### Linux / macOS
```bash
git clone https://github.com/0thernes/cellular-engine-omega.git
cd cellular-engine-omega

# Build native C11 (-O3 -Werror -ffp-contract=off), run ASan/UBSan, 60 gates, 16/16 mutants,
# 256 fuzz cases, 75 substrates + 1,000 Hybrid Cube, live C11->Lean export & 89 Lean 4.35.0 proofs
OMEGA_SANITIZE=1 ./build_linux.sh

# Isolated Challenge/Solution multi-kernel comparator (strict constructive allowlist)
(cd lean && lake comparator --config comparator.json --paranoid)

# Independent Python verification ladder (35 tests, 1,200+ Hypothesis property cases, exact oracle)
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

# 5. Launch hardened loopback-only Web Observatory (http://127.0.0.1:8787)
node web/server.mjs
```

Runnable consumer scripts live in [`examples/`](examples/):
- [`examples/quickstart_demo.py`](examples/quickstart_demo.py) — Python JSON API client
- [`examples/c11_abi_example.c`](examples/c11_abi_example.c) — Direct C11 static/shared ABI linkage (`include/omega_abi.h`)
- [`examples/web_api_demo.mjs`](examples/web_api_demo.mjs) — Node.js client for `/api/health`, `/healthz`, and `/api/cube`

---

## Verified Key Metrics (`v5.2.0`, Seed `2026001`)

| Layer | Verified Metric | Evidence Artifact |
| :--- | :--- | :--- |
| **C11 Falsification** | **`60 / 60` gates PASS · `16 / 16` planted mutants caught · `256 / 256` fuzz PASS** | [`results/falsification_win64.txt`](results/falsification_win64.txt) |
| **Optimization Parity** | **`-O0` == `-O3` bit-exact Master SHA-256** (`a92641843e46736b...`, `90.7 ms` vs `340.7 ms`) | [`docs/benchmarks.md`](docs/benchmarks.md) |
| **75 Substrates & 1,000 Cube** | **`75 / 75` (`25 PM + 25 MF + 25 GW`) · `661 [OK] / 274 [ADAPT] / 20 [OVER] / 45 [EXCL]` · `20 / 20` seeds** | [`results/omega_75_substrates_1000_cube.json`](results/omega_75_substrates_1000_cube.json) |
| **Lean 4.35.0 Proofs** | **`89 / 89` strictly constructive (`17` axiom-free, `10` `[propext]`, `62` `[propext, Quot.sound]`, `0` `Classical.choice`, `0` Mathlib)** | [`results/lean4-omega-audit.txt`](results/lean4-omega-audit.txt) |
| **Multi-Kernel Audit** | **`lean`, `leanchecker`, `leanchecker-paranoid`, `con-leche --verified` (`58,614` decls), `con-ron --verified`, Linux CI `lake comparator --paranoid`** | [`lean/comparator.json`](lean/comparator.json) |
| **Python Ladder & Stats** | **`35 / 35` pytest (`1,200+` Hypothesis cases) · `8 / 8` preregistered `N=20` contrasts (`p_Holm = 0.0008 < 0.001`)** | [`results/multiseed_stats.csv`](results/multiseed_stats.csv) |
| **Mega-Atlas Cross-Check** | **`0 / 1,900` overarching open problems retired (22 adjacent parent problems remain strictly OPEN)** | [`docs/mega-atlas-cross-check.md`](docs/mega-atlas-cross-check.md) |

---

## Verbatim Lean 4.35.0 `#print axioms` Excerpt (CI Drift-Checked)

Audited mechanically by [`scripts/check_readme_axioms.py`](scripts/check_readme_axioms.py) against [`results/lean4-omega-audit.txt`](results/lean4-omega-audit.txt) (all 89 theorems strictly within `{propext, Quot.sound}` — zero `Classical.choice`, zero `sorryAx`, zero `native_decide`):

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
```
<!-- LEAN_AXIOMS_END -->

---

## Honest Verification Boundary & Limitations

- **What IS formally proved in Lean 4.35.0:** Exact `Nat`/`Int`/`List` algebraic, combinatorial, and inductive cellular biophysics invariants (`O01`–`O62`, `O67`–`O71`), 17 non-vacuity and sharp negative-boundary refutations (`O73`–`O89`), and kernel verification of concrete integer/string fields in live C11 receipts (`O63`, `O72`).
- **What is NOT proved in Lean:** IEEE-754 floating-point PDE convergence (Cahn–Hilliard, Schnakenberg, spherical nematic relaxation) and C11 compiler memory safety — these are verified empirically by 60 gates, 16 compiled structural knockouts, ASan/UBSan, a 256-case fuzz harness, `-O0` vs `-O3` parity, and an exact Python reference oracle.
- **No Overclaimed Breakthroughs:** Cross-checked against all 1,900 problems in [`0thernes/unsolved-problems-mega-atlas`](https://github.com/0thernes/unsolved-problems-mega-atlas): **`0 / 1,900`** overarching open problems are solved. Full real-vs-metaphor audit in [`LIMITATIONS.md`](LIMITATIONS.md) and [`CLAIMS.md`](CLAIMS.md).
- **AI-Assistance Transparency:** AI engineering tools assisted with implementation, tactic exploration, drafting, and red-team auditing under sole human direction and accountability ([`docs/AI_DISCLOSURE.md`](docs/AI_DISCLOSURE.md)).

---

## Documentation Map

- **Core Ledgers:** [`CLAIMS.md`](CLAIMS.md) · [`LIMITATIONS.md`](LIMITATIONS.md) · [`VERIFICATION.md`](VERIFICATION.md) · [`PREREGISTRATION.md`](PREREGISTRATION.md) · [`REPLICATION.md`](REPLICATION.md) · [`HARDENING_LOG.md`](HARDENING_LOG.md) · [`ERRATA.md`](ERRATA.md) · [`DEVIATIONS.md`](DEVIATIONS.md)
- **Technical Reference:** [`docs/architecture.md`](docs/architecture.md) · [`docs/statement-fidelity.md`](docs/statement-fidelity.md) · [`docs/mega-atlas-cross-check.md`](docs/mega-atlas-cross-check.md) · [`docs/benchmarks.md`](docs/benchmarks.md) · [`docs/threat-model.md`](docs/threat-model.md) · [`docs/operations.md`](docs/operations.md) · [`docs/configuration.md`](docs/configuration.md) · [`docs/faq.md`](docs/faq.md) · [`docs/glossary.md`](docs/glossary.md) · [`docs/adr/README.md`](docs/adr/README.md)
- **White Paper & Blueprint:** [`paper/main.tex`](paper/main.tex) · [`dist/Donahue_2026_cellular_engine_omega.pdf`](dist/Donahue_2026_cellular_engine_omega.pdf) · [`blueprint/index.html`](blueprint/index.html)

---

## Citation & License

- **Code, Lean 4 Proofs & Scripts:** [Apache License 2.0](LICENSE)
- **White Paper, Figures, Datasets & Prose Docs:** [Creative Commons Attribution 4.0 International (CC BY 4.0)](LICENSE-DOCS)
- **Citation Metadata:** See [`CITATION.cff`](CITATION.cff) and [`CITATION.bib`](CITATION.bib).
