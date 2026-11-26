# Frequently Asked Questions (`A44`)

### 1. Does CELLULAR ENGINE OMEGA solve any of the 1,900 open problems in `unsolved-problems-mega-atlas`?
**No — `0 / 1,900`.** All 22 directly adjacent open problems in biology, chemistry, fluid dynamics, active matter, neuroscience, AI, and computer architecture remain strictly **OPEN**. See [`docs/mega-atlas-cross-check.md`](mega-atlas-cross-check.md).

### 2. What do the 89 Lean 4.35.0 proofs actually prove, and what do they NOT prove?
They constructively prove exact integer, natural-number, and list algebraic/combinatorial invariants (`O01`–`O62`, `O67`–`O71`), non-vacuity and sharp negative-boundary refutations (`O73`–`O89`), and kernel-checked consistency of live C11 telemetry receipts (`O63`, `O72`) strictly within `{propext, Quot.sound}` (`0` `Classical.choice`, `0` `sorryAx`, `0` Mathlib). They do **not** formally prove IEEE-754 floating-point PDE convergence or C11 compiler memory safety (which are verified empirically via 60 gates, 16 planted mutants, ASan/UBSan, fuzzing, and the Python oracle). See [`docs/statement-fidelity.md`](statement-fidelity.md).

### 3. Why did `v5.2.0` change `cube_sha256` while keeping `master_sha256` identical?
Our new multi-seed Cube gate `G57` exposed a real `v5.1.0` variance bug in `PM21` (single-trial stochastic rounding failed on seeds `2026003` and `2026006`, `1.31-sigma`). Upgrading `PM21` to an unbiased 16-trial estimator (`5.26-sigma`) deterministically updated `cube_sha256` while preserving `master_sha256` (`TB01`–`TB16`) bit-for-bit. See [`ERRATA.md`](../ERRATA.md) and [`UPGRADING.md`](../UPGRADING.md).

### 4. Why is there zero Mathlib dependency in Lean 4?
Zero Mathlib keeps the trusted proof base tiny (~2.5 s per module build), eliminates third-party proof drift, enables standalone multi-kernel checking (`leanchecker-paranoid`, `con-leche --verified`, `con-ron --verified`, `nanoda_bin`, `lake comparator --paranoid`) in seconds, and makes every single theorem auditable end-to-end.

### 5. Can I deploy `web/server.mjs` to the public internet?
**No.** It is intentionally hardened for **local loopback (`127.0.0.1`)** exploration only, with no authentication layer. For public browsing, use the static GitHub Pages showcase (`pages/` + `blueprint/`).

### 6. How was AI used in this project?
AI coding/research tools assisted with implementation, tactic exploration, drafting, and adversarial auditing under strict human direction and machine-checked verification. Full disclosure is in [`docs/AI_DISCLOSURE.md`](AI_DISCLOSURE.md).
