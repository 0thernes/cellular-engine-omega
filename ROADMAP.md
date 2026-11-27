# Roadmap

CELLULAR ENGINE OMEGA follows a verification-first roadmap prioritizing mathematical rigor, cross-platform numerical determinism, and honest scoping over speculative feature sprawl.

## Completed Milestones

- **v1.0–v4.2:** Core 12-testbed C11 biophysics engine, `TB13` 12-axis morphospace, `TB14` Bio-Frostbite Lafont Interaction Net VM, and initial Lean 4 proofs (`O01`–`O46`).
- **v5.0–v5.1:** `TB15` Deep-Strata Frontier, `TB16` Virtual-Cell AI & JCVI-syn3A manifold, live C11-to-Lean runtime receipts (`O63`, `O72`), and Clean-Slate v3.0 75 Substrates + 1,000-Cell Hybrid Cube.
- **v5.2.0:** Constructive 89-theorem Lean 4.35.0 suite across 6 independent kernels (isolated `Spec.lean`/`Challenge.lean`/`Solution.lean` comparator), 60 C11 falsification gates + 16/16 planted mutants + 256-case fuzz harness, `PM21` multi-seed variance fix, independent Python exact oracle & statistical ladder, AMS-LaTeX white paper + vector figures + Lean Proof Blueprint, and 100% Build Standard v1.2.0 compliance.
- **v5.3.0:** Expanded Lean 4.35.0 suite to **100/100 constructive proofs** (`AdversarialTests.lean` `O90`–`O100`, banning `Classical.choice`, 6-kernel verification + negative elaboration test), corrected PRNG docs to `Philox4x32-10` and Web Observatory default port to `8791`, added FAIR metadata (`croissant.json`, `ro-crate-metadata.json`, `codemeta.json`, `docs/DATASHEET.md`), DigiCert RFC 3161 `.tsr` timestamps, and CI Tectonic monograph + clean-room arXiv verification.
- **v5.4.0 (Current):** Fixed `nuchter2006nanosecond` DOI (`10.1016/j.cub.2006.03.089`) and upgraded Crossref citation audit (`32/32` title + first-author match, `0` retractions), tightened `O01`/`O56` hypotheses and honest statement-fidelity annotations, closed all 5 frozen paper errata (`0` overfull hboxes), added live negative axiom-gate + comparator `sorry`-tamper rejection (`scripts/test_lean_negative.py`), REUSE metadata + dual DigiCert `.tsr` + 3-calendar Bitcoin OTS `.ots` timestamps, and upgraded workflows to Node 24.

## Planned Milestones

- **v5.5 (Cross-Architecture & PDE Convergence):**
  - Native ARM64 (`linux-aarch64` / `macos-arm64`) bit-exact SHA-256 verification receipts alongside x86-64.
  - Multi-resolution mesh refinement study (`N = 32, 64, 128`) for `TB04` Cahn–Hilliard LLPS and `TB07` Schnakenberg Turing PDEs with measured spatial convergence order.
  - Optional CBMC bounded model checking pass (`B25`) on `src/omega_rng_hash.c` and `src/exp_14_bio_inet.c` pointer/index safety.
- **v6.0 (Formal-Numerical Bridge Expansion):**
  - Interval-arithmetic certificates linking selected IEEE-754 C11 PDE step bounds to Lean 4 rational bounds.
  - External independent replication receipts from third-party contributors (tracked in `REPLICATION.md`).

## Explicit Non-Goals

- Claiming to solve or retire overarching open problems in biology, physics, or computer science (see `docs/mega-atlas-cross-check.md` — `0 / 1,900` Mega-Atlas problems retired).
- Adding opaque runtime cloud APIs, external ML model downloads, or non-deterministic network dependencies.
