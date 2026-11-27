module
-- SPDX-License-Identifier: Apache-2.0
-- Copyright (c) 2026 Alexander Stephen Donahue

public import OmegaCellularProofs.LiveC11Receipt

/-!
# Spec.lean — Pure Definitions & Data Specification Module (v5.4.0)

Contains ONLY pure data structures and computable definitions (ZERO proofs, ZERO `sorry`)
shared between isolated statement-only `Challenge.lean` and the 14 sorry-free proof modules
in `OmegaCellularProofs.*` / `Solution.lean` (100 strictly constructive theorems, 0 `Classical.choice`).
Keeping definitions in a proof-free `Spec` module guarantees that `Challenge.lean` imports zero
proof terms and can be judged against `Solution.lean` under
`lake comparator --config comparator.json --paranoid`.
-/

namespace OmegaCellular.PurcellGaugeGeometry
@[expose] public section

/-- Total low-Reynolds Stokes displacement along a discrete shape-space path under
    an antisymmetric gauge connection `A`. -/
def pathDisp (A : Int → Int → Int) : List Int → Int
  | [] => 0
  | [_] => 0
  | x :: y :: rest => A x y + pathDisp A (y :: rest)

/-- Twice the signed loop area (`2 * ∮ x dy`) of a rectangular 2-DOF cycle
    `(a, b) -> (a + da, b) -> (a + da, b + db) -> (a, b + db) -> (a, b)`. -/
def twoDofLoopArea2 (a b da db : Int) : Int :=
  let x0 := a;       let y0 := b
  let x1 := a + da;  let y1 := b
  let x2 := a + da;  let y2 := b + db
  let x3 := a;       let y3 := b + db
  (x0 * y1 - x1 * y0) +
  (x1 * y2 - x2 * y1) +
  (x2 * y3 - x3 * y2) +
  (x3 * y0 - x0 * y3)

end
end OmegaCellular.PurcellGaugeGeometry

namespace OmegaCellular.TuringFeinbergCRNT
@[expose] public section

/-- Feinberg CRNT deficiency `δ = n - l - s` (complexes `n`, linkage classes `l`,
    stoichiometric subspace dimension `s`). -/
def feinbergDeficiency (n l s : Nat) : Nat := n - l - s

end
end OmegaCellular.TuringFeinbergCRNT

namespace OmegaCellular.TURSpeedLimitsTopo
@[expose] public section

/-- Twice the total topological defect charge (`2 * ∑ q_i`), so `+1/2` nematic disclinations
    are represented by exact integer `1`, `+1` asters by `2`, and `S²` Euler characteristic `χ = 2`
    corresponds to exact integer `4`. -/
def defectChargeSum2 : List Int → Int
  | [] => 0
  | q :: qs => q + defectChargeSum2 qs

end
end OmegaCellular.TURSpeedLimitsTopo

namespace OmegaCellular.BioInteractionNets
@[expose] public section

/-- 6-compartment linear-resource state of the 4-layer Bio-Frostbite Interaction Net VM. -/
structure BioNetState where
  dnaMass       : Int
  organelleMass : Int
  cellMass      : Int
  creatureMass  : Int
  atpPool       : Int
  dissipated    : Int
  deriving Repr, DecidableEq

/-- Conserved total mass-energy across all 6 linear-ownership compartments. -/
def totalMass (s : BioNetState) : Int :=
  s.dnaMass + s.organelleMass + s.cellMass + s.creatureMass + s.atpPool + s.dissipated

/-- Four biological Lafont active-pair rewrite rules spanning DNA, Organelle, Cell, and Creature layers. -/
inductive ActivePairRule where
  | polyDup       (delta : Int)
  | riboTranslate (delta : Int)
  | cellFission   (delta : Int)
  | gapJunction   (delta : Int)
  deriving Repr, DecidableEq

/-- Single deterministic active-pair rewrite conserving `totalMass`. -/
def stepNet (r : ActivePairRule) (s : BioNetState) : BioNetState :=
  match r with
  | .polyDup d =>
      { s with
        dnaMass       := s.dnaMass - d
        organelleMass := s.organelleMass + d }
  | .riboTranslate d =>
      { s with
        organelleMass := s.organelleMass - d
        cellMass      := s.cellMass + d }
  | .cellFission d =>
      { s with
        cellMass      := s.cellMass - d
        creatureMass  := s.creatureMass + d }
  | .gapJunction d =>
      { s with
        atpPool       := s.atpPool - d
        dissipated    := s.dissipated + d }

/-- Execute an arbitrary sequence of active-pair rewrites. -/
def runTrace : List ActivePairRule → BioNetState → BioNetState
  | [], s => s
  | r :: rs, s => runTrace rs (stepNet r s)

/-- Local disjoint site update on an indexed interaction-net memory map. -/
def applyLocalPair (idx : Nat) (delta : Int) (net : Nat → Int) : Nat → Int :=
  fun k => if k = idx then net k + delta else net k

/-- Dynamic power consumption of a Bio-Frostbite interaction net under dry/hydrated state. -/
def cryptobioticPower (dry : Bool) (hydratedPairs powerPerPair : Nat) : Nat :=
  if dry then 0 else hydratedPairs * powerPerPair

/-- Nested 3-genome Russian-doll endosymbiosis box (host + 3 private membrane depths). -/
structure RussianDollBoxes where
  hostPrivate   : Int
  depth1Private : Int
  depth2Private : Int
  depth3Private : Int
  deriving Repr, DecidableEq

/-- Rewrite confined strictly inside the innermost endosymbiont membrane boundary (`depth2 <-> depth3`). -/
def rewriteInnerBox (b : RussianDollBoxes) (q : Int) : RussianDollBoxes :=
  { b with
    depth2Private := b.depth2Private - q
    depth3Private := b.depth3Private + q }

end
end OmegaCellular.BioInteractionNets

namespace OmegaCellular.DeepStrataFrontier
@[expose] public section

/-- Total base-pair length of a list of DNA segments (MDS or IES fragments). -/
def sumBp : List Nat → Nat
  | [] => 0
  | x :: xs => x + sumBp xs

/-- Levin bioelectric tissue polarity state (identical genome, anterior/posterior `Vmem`, threshold). -/
structure BioelectricTissue where
  dna        : Nat
  vAnterior  : Int
  vPosterior : Int
  vThreshold : Int
  deriving Repr, DecidableEq

/-- Number of head poles induced by anterior/posterior membrane potentials exceeding `vThreshold`. -/
def headCount (t : BioelectricTissue) : Nat :=
  (if t.vAnterior > t.vThreshold then 1 else 0) +
  (if t.vPosterior > t.vThreshold then 1 else 0)

end
end OmegaCellular.DeepStrataFrontier

namespace OmegaCellular.VirtualCellManifold
@[expose] public section

/-- Exact integer sum of a flux/delta vector. -/
def sumInt : List Int → Int
  | [] => 0
  | x :: xs => x + sumInt xs

/-- Exact integer scaled hyperplane projection `∑_i (N * x_i - S)` avoiding division rounding. -/
def sumScaledShift (N S : Int) : List Int → Int
  | [] => 0
  | x :: xs => (N * x - S) + sumScaledShift N S xs

/-- Conservative pairwise transfer across adjacent tiers in the 6-tier stack. -/
def transferPair (m1 m2 f : Int) : Int × Int := (m1 - f, m2 + f)

end
end OmegaCellular.VirtualCellManifold

namespace OmegaCellular.VerifyTraceReceipt
@[expose] public section

/-- Coupled 6-compartment mass + 2-node bioelectric charge + cryptobiotic tun trace state. -/
structure BioTraceState where
  dnaMass       : Int
  organelleMass : Int
  cellMass      : Int
  creatureMass  : Int
  fuelPool      : Int
  heatPool      : Int
  chargeA       : Int
  chargeB       : Int
  rwCount       : Nat
  tunFrozen     : Bool
  deriving Repr, DecidableEq

/-- Total conserved mass across all 6 trace compartments. -/
def totalMass (s : BioTraceState) : Int :=
  s.dnaMass + s.organelleMass + s.cellMass + s.creatureMass + s.fuelPool + s.heatPool

/-- Total conserved bioelectric charge across gap-junction coupled compartments. -/
def totalCharge (s : BioTraceState) : Int :=
  s.chargeA + s.chargeB

/-- Trace events matching the C11 `TB14` Bio-Frostbite VM + cryptobiotic tun transitions. -/
inductive BioTraceEvent where
  | dnaToOrg     (d : Int)
  | orgToCell    (d heat : Int)
  | cellToCreat  (d heat : Int)
  | fuelBurn     (work heat : Int)
  | gapExchange  (dq : Int)
  | tunDryStep
  | tunRehydrate
  deriving Repr, DecidableEq

/-- Single trace transition step obeying cryptobiotic freeze and exact conservation. -/
def stepTrace (s : BioTraceState) (ev : BioTraceEvent) : BioTraceState :=
  match ev with
  | .dnaToOrg d =>
      if s.tunFrozen then s
      else { s with
        dnaMass := s.dnaMass - d
        organelleMass := s.organelleMass + d
        rwCount := s.rwCount + 1 }
  | .orgToCell d heat =>
      if s.tunFrozen then s
      else { s with
        organelleMass := s.organelleMass - d
        cellMass := s.cellMass + d
        fuelPool := s.fuelPool - heat
        heatPool := s.heatPool + heat
        rwCount := s.rwCount + 1 }
  | .cellToCreat d heat =>
      if s.tunFrozen then s
      else { s with
        cellMass := s.cellMass - d
        creatureMass := s.creatureMass + d
        fuelPool := s.fuelPool - heat
        heatPool := s.heatPool + heat
        rwCount := s.rwCount + 1 }
  | .fuelBurn work heat =>
      if s.tunFrozen then s
      else { s with
        fuelPool := s.fuelPool - work - heat
        cellMass := s.cellMass + work
        heatPool := s.heatPool + heat
        rwCount := s.rwCount + 1 }
  | .gapExchange dq =>
      if s.tunFrozen then s
      else { s with
        chargeA := s.chargeA - dq
        chargeB := s.chargeB + dq
        rwCount := s.rwCount + 1 }
  | .tunDryStep =>
      { s with tunFrozen := true }
  | .tunRehydrate =>
      { s with tunFrozen := false }

/-- Replay a full trace of events from an initial state. -/
def replayTrace (s : BioTraceState) : List BioTraceEvent → BioTraceState
  | [] => s
  | ev :: rest => replayTrace (stepTrace s ev) rest

/-- Predicate identifying events that do not rehydrate a cryptobiotically frozen tun. -/
def isNonRehydrate : BioTraceEvent → Bool
  | .tunRehydrate => false
  | _ => true

end
end OmegaCellular.VerifyTraceReceipt
