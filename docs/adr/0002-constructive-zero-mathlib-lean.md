# ADR 0002: Constructive Zero-Mathlib Lean 4.35.0 with Isolated `Spec`/`Challenge`/`Solution` & Multi-Kernel Auditing

- **Status:** Accepted (`v5.2.0`)
- **Date:** `2026-11-25`

## Context
Formal proof artifacts in independent AI-assisted research are vulnerable to three failure modes: (i) huge transitive dependencies (Mathlib) obscuring the trusted kernel base, (ii) subtle statement drift or vacuous hypotheses when challenge statements and proofs share mutable modules, and (iii) reliance on a single typechecker or unnecessary `Classical.choice` axioms.

## Decision
1. Build all `89` theorems (`O01`–`O89`) in pure zero-Mathlib Lean 4.35.0 using the `module` system with `autoImplicit = false`, `relaxedAutoImplicit = false`, `linter.unusedVariables = true`, and `linter.deprecated = true`.
2. Separate pure definitions into proof-free `OmegaCellularProofs/Spec.lean`, statement-only `Challenge.lean` (imports ONLY `Spec`), and sorry-free `Solution.lean` (`OmegaCellularProofs`), judged via `lake comparator --config comparator.json --paranoid`.
3. Eliminate all `Classical.choice` usage by splitting constructive conjunctions/biconditionals, keeping `100%` of theorems strictly within `{propext, Quot.sound}` (or axiom-free).
4. Add `17` explicit non-vacuity and sharp negative-boundary theorems (`NonVacuity.lean`, `O73`–`O89`).
5. Audit across independent kernels: `lean`, `leanchecker`, `leanchecker-paranoid`, `con-leche --verified`, `con-ron --verified`, and Linux CI `nanoda_bin` + `lake comparator --paranoid`.

## Consequences
- **Positive:** Minimal trusted kernel base, `< 20 s` full build, airtight statement-to-proof separation, and zero classical/compiler/SAT axioms.
- **Negative:** Formalization scope is intentionally restricted to exact `Nat`/`Int`/`List` algebraic invariants and runtime receipts rather than real-analysis PDE discretization bounds (disclosed in `docs/statement-fidelity.md`).
