# Limitations, Scope Boundaries & Real-vs-Metaphor Audit (`B41`)

This document states clearly what **CELLULAR ENGINE OMEGA (`v5.2.0`)** actually verifies, what is modeled only as a reduced-order or synthetic surrogate, and what it explicitly **does not** prove or solve.

---

## 1. Real vs. Metaphor & Abstraction Table

| Component | What Is Actually Executed / Proved | What It Is **Not** (Explicit Non-Claim) |
| :--- | :--- | :--- |
| **Lean 4.35.0 Proofs (`O01`–`O89`)** | Exact machine-checked algebraic, combinatorial, and inductive theorems over `Nat`/`Int`/`List` (`0` Mathlib, constructive `{propext, Quot.sound}`), plus kernel checking of concrete integer/string C11 telemetry fields (`O63`, `O72`). | **Not** a formal proof of IEEE-754 floating-point PDE convergence, numerical stability of Cahn–Hilliard/Schnakenberg solvers, or C11 compiler/memory safety. |
| **C11 Biophysics Solvers (`TB01`–`TB12`, `TB15`)** | Deterministic low-dimensional ODE, 2D PDE, spherical mesh, Gillespie SSA, and Monte Carlo reference implementations demonstrating textbook scaling laws and caught structural knockouts (`M1`–`M16`). | **Not** atomistic molecular dynamics, ab initio quantum chemistry, or clinical/wet-lab validated predictors of living *Hypsibius exemplaris* or human cells. |
| **`TB09` "Walking-Head" Tardigrade (`<= 1,000` cells)** | Reduced-order 7-organ eutelic morphogenetic & CPG locomotion/radiotolerance simulator verifying budget constraints (`780 <= 1,000`) and 3-tier multiplicative survival ratios. | **Not** a biochemically complete 3D developmental embryo or a wet-lab radiobiology model. |
| **`TB14` "Bio-Frostbite" Interaction Nets** | Deterministic C11 Lafont interaction-net virtual machine proving schedule-independent strong confluence (`FIFO == LIFO == Random` SHA-256), integer linear conservation, and zero-active-pair cryptobiotic freezing. | **Not** fabricated ASIC silicon or taped-out GDSII hardware; "Bio-Frostbite" is an architectural software model inspired by interaction-net hardware semantics. |
| **`TB16` "Virtual-Cell AI" & JCVI-syn3A (`493` / `92` genes)** | Synthetic benchmark manifold (`N` held-out perturbation splits) evaluating `L2`-regularized linear Ridge regression + exact `O56` hyperplane conservation projection, parameterized by the published JCVI-syn3A gene/dark-proteome counts (`493` total, `452` protein-coding, `92` uncharacterized). | **Not** a pretrained billion-parameter foundation model trained on experimental Perturb-seq/scRNA-seq corpora, and does **not** experimentally annotate the 92 biological JCVI-syn3A genes of unknown function. |
| **75 Substrates & 1,000-Cell Hybrid Cube (`omega_substrates_cube.c`)** | Executable C11 test harness checking 25 numeric/precision codecs (`PM01`–`PM25`), 25 model-family kernels (`MF01`–`MF25`), 25 geometry/topology builders (`GW01`–`GW25`), and 1,000 `10x10x10` composition triples under a strict `Q <= 10` (`16 KiB`) statevector cap. | **Not** a fault-tolerant quantum computer simulator beyond `10` qubits, nor a full production compiler for arbitrary hybrid neural/PDE workloads. |
| **Mega-Atlas Cross-Check (`0 / 1,900` solved)** | Honest scoped mapping to `22` directly adjacent open problems in `0thernes/unsolved-problems-mega-atlas`. | Solves **`0 / 1,900`** overarching open problems; every parent biological, physical, and computational problem remains **OPEN**. |

---

## 2. Technical & Platform Limitations

1. **Floating-Point Portability:** Cross-OS reproducibility between Win64 Clang and Linux GCC is enforced via `-ffp-contract=off` and verified in CI on `x86_64`. Bit-exact SHA-256 identity across `aarch64` (ARM64) or alternate `libm` implementations (e.g., `musl` vs `glibc` vs `ucrtbase` transcendental `exp`/`sin`/`cos` last-ULP differences) is **not** guaranteed for floating-point testbeds (`TB01`–`TB13`, `TB15`–`TB16`), whereas integer/combinatorial `TB14` strong confluence and Lean 4.35.0 theorems are strictly architecture-independent.
2. **Local Windows `lake comparator` / `lean4lean`:** On Windows, `lake comparator` requires Linux `bwrap` namespaces, and the bundled `lean4lean` binary exhibits an `.olean` header mismatch against Lean 4.35.0 `Std`. Locally on Windows, **4 independent non-reference checkers** (`lean`, `leanchecker`, `leanchecker-paranoid`, `con-leche --verified`, `con-ron --verified`) pass with exit `0`; on Ubuntu Linux CI, `lake comparator --config comparator.json --paranoid` and `nanoda_bin` additionally verify the isolated `Challenge`/`Solution` boundary.
3. **Web Observatory Deployment Model:** `web/server.mjs` is strictly designed for **local loopback (`127.0.0.1`)** interactive inspection. It has no authentication layer and must never be exposed directly to the public internet.
4. **Statistical Power & Synthetic Noise:** The `20`-seed statistical suite (`results/multiseed_stats.csv`) evaluates deterministic synthetic PRNG ensembles, not wet-lab biological replicates.
5. **Preprint Status:** Neither the software nor `paper/main.tex` has undergone external journal peer review.
