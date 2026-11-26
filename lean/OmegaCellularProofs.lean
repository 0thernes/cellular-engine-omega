module
-- SPDX-License-Identifier: Apache-2.0
-- Copyright (c) 2026 Alexander Stephen Donahue

public import OmegaCellularProofs.LiveC11Receipt
public import OmegaCellularProofs.Spec
public import OmegaCellularProofs.PurcellGaugeGeometry
public import OmegaCellularProofs.EigenProofreadingInfo
public import OmegaCellularProofs.TuringFeinbergCRNT
public import OmegaCellularProofs.TURSpeedLimitsTopo
public import OmegaCellularProofs.CytoskeletonMotorsLLPS
public import OmegaCellularProofs.TardigradeVirosphereOmega
public import OmegaCellularProofs.OpenProliferationWeb
public import OmegaCellularProofs.BioInteractionNets
public import OmegaCellularProofs.DeepStrataFrontier
public import OmegaCellularProofs.VirtualCellManifold
public import OmegaCellularProofs.VerifyTraceReceipt
public import OmegaCellularProofs.SubstratesCube
public import OmegaCellularProofs.NonVacuity

/-!
# OmegaCellularProofs.lean — Root Formal Verification Library (v5.2.0)

Aggregates all 13 pure Lean 4.35.0 `module` proof & specification files exporting
**89 machine-checked frontier theorems** (`O01`–`O63` + `O01b` + `O41b` + `O59b` +
`O67`–`O72` + `O73`–`O89` non-vacuity & sharp negative-boundary witnesses) with:
- Zero Mathlib dependencies,
- Zero `sorry`, zero `admit`, zero `native_decide`, zero compiler/SAT axioms,
- Strict `autoImplicit = false`, `relaxedAutoImplicit = false`,
  `linter.unusedVariables = true`, `linter.deprecated = true`.

Axiom auditing (`#print axioms`) is run exclusively via standalone `VerifyAxioms.lean`
(`lake env lean VerifyAxioms.lean > ..\results\lean4-omega-audit.txt`) so library builds
emit zero stdout noise while every declaration's exact axiom footprint is archived.
-/
