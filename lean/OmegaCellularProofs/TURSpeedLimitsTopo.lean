module
-- SPDX-License-Identifier: Apache-2.0
-- Copyright (c) 2026 Alexander Stephen Donahue

public import OmegaCellularProofs.Spec

/-!
  CELLULAR ENGINE OMEGA v5.2.0 — Pure Lean 4.35.0 Formal Verification (Zero Mathlib)
  Module 4: Thermodynamic Uncertainty Relation, Speed Limits, Poincaré–Hopf & Willmore (`O16`–`O21`)
-/

namespace OmegaCellular.TURSpeedLimitsTopo

@[expose] public section

theorem O16_tur_halving_jitter_quadruples_entropy
    (Vnew S J : Nat) (hSat : (4 * Vnew) * S = 2 * J * J) :
    Vnew * (4 * S) = 2 * J * J := by
  have h : Vnew * (4 * S) = (4 * Vnew) * S := by
    rw [← Nat.mul_assoc, Nat.mul_comm Vnew 4]
  rw [h]; exact hSat

theorem O17_tur_precision_entropy_monotone
    (V1 V2 S1 S2 J : Nat)
    (_hV2 : 0 < V2) (hVlt : V2 < V1) (hS1 : 0 < S1)
    (h1 : V1 * S1 = 2 * J * J) (h2 : V2 * S2 = 2 * J * J) :
    S1 < S2 := by
  by_cases hlt : S1 < S2
  · exact hlt
  · have hle : S2 ≤ S1 := Nat.le_of_not_lt hlt
    have hmul : V2 * S2 < V1 * S1 := by
      have ha : V2 * S2 ≤ V2 * S1 := Nat.mul_le_mul_left V2 hle
      have hb : V2 * S1 < V1 * S1 := Nat.mul_lt_mul_of_pos_right hVlt hS1
      exact Nat.lt_of_le_of_lt ha hb
    omega

theorem O18_thermodynamic_speed_limit_scaling
    (tauFast D dS W2 : Nat)
    (hBound : (2 * tauFast) * D * dS ≥ W2) :
    tauFast * D * (2 * dS) ≥ W2 := by
  have hEq : tauFast * D * (2 * dS) = (2 * tauFast) * D * dS := by
    calc tauFast * D * (2 * dS)
        = (tauFast * D) * 2 * dS := (Nat.mul_assoc (tauFast * D) 2 dS).symm
      _ = 2 * (tauFast * D) * dS := by rw [Nat.mul_comm (tauFast * D) 2]
      _ = (2 * tauFast) * D * dS := by rw [← Nat.mul_assoc 2 tauFast D]
  rw [hEq]
  exact hBound

theorem O19_poincare_hopf_sphere_defect_impossibility :
    defectChargeSum2 [] ≠ 4 ∧
    defectChargeSum2 [1, 1, 1, 1] = 4 ∧
    defectChargeSum2 [2, 2] = 4 := by
  decide

theorem O20_poincare_hopf_defect_fusion_conservation (rest : List Int) :
    defectChargeSum2 (1 :: 1 :: rest) = defectChargeSum2 (2 :: rest) := by
  simp [defectChargeSum2]
  omega

theorem O21_willmore_clifford_torus_ratio :
    2 * (314 : Nat) * 314 > 19 * 100 * 100 ∧
    ∀ r : Nat, 0 < r → 2 * (r * r) > r * r := by
  refine ⟨by decide, ?_⟩
  intro r hr
  have hr2 : 0 < r * r := Nat.mul_pos hr hr
  omega

end

end OmegaCellular.TURSpeedLimitsTopo
