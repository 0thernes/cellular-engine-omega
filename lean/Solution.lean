module
-- SPDX-License-Identifier: Apache-2.0
-- Copyright (c) 2026 Alexander Stephen Donahue

public import OmegaCellularProofs

/-!
# Solution.lean — Isolated Formal Solution Entrypoint (v5.4.0)

Exports all 100 strictly constructive, sorry-free machine-checked theorems (`0 Classical.choice`)
from `OmegaCellularProofs` (14 pure Lean 4.35.0 `module` proof files, zero Mathlib,
`autoImplicit = false`) for isolated statement-to-proof judging against `Challenge.lean`
using the reference kernel (`lean`, `leanchecker`, `leanchecker-paranoid`) plus four independent
checkers (`lean4lean`, `nanoda_bin`, `con-leche`, `con-ron`) via:
    lake comparator --config comparator.json --paranoid
-/
