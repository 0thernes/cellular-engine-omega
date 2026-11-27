module
-- SPDX-License-Identifier: Apache-2.0
-- Copyright (c) 2026 Alexander Stephen Donahue

public import OmegaCellularProofs.Spec

/-!
  CELLULAR ENGINE OMEGA v5.4.0 — Pure Lean 4.35.0 Formal Verification (Zero Mathlib)
  Module 3 of 14 (100 strictly constructive theorems, 0 `Classical.choice`):
  Feinberg CRNT Deficiency & 2×2 Integer Turing Jacobian Algebra (`O11`–`O15`)
-/

namespace OmegaCellular.TuringFeinbergCRNT

@[expose] public section

theorem O11_feinberg_deficiency_nonneg (n l s : Nat) (hBound : l + s ≤ n) :
    feinbergDeficiency n l s + l + s = n ∧ (feinbergDeficiency n l s = 0 ↔ n = l + s) := by
  unfold feinbergDeficiency
  exact ⟨by omega, ⟨fun h => by omega, fun h => by omega⟩⟩

theorem O12_turing_equal_diffusion_impossibility
    (a b c d D k2 : Int)
    (hTr : a + d < 0)
    (hDet : 0 < a * d - b * c)
    (hD : 0 < D) (hk : 0 < k2) :
    (a - D * k2) + (d - D * k2) < 0 ∧
    0 < (a - D * k2) * (d - D * k2) - b * c := by
  have hDk : 0 < D * k2 := Int.mul_pos hD hk
  refine ⟨by omega, ?_⟩
  have hExp : (a - D * k2) * (d - D * k2) - b * c =
      (a * d - b * c) + (-(a + d)) * (D * k2) + (D * k2) * (D * k2) := by
    have h1 : (a - D * k2) * (d - D * k2) =
        a * d - a * (D * k2) - (D * k2) * d + (D * k2) * (D * k2) := by
      rw [Int.sub_mul, Int.mul_sub, Int.mul_sub]
      omega
    have h2 : (-(a + d)) * (D * k2) = -(a * (D * k2)) - d * (D * k2) := by
      rw [Int.neg_mul, Int.add_mul]
      omega
    have h3 : (D * k2) * d = d * (D * k2) := Int.mul_comm (D * k2) d
    omega
  rw [hExp]
  have hNegTr : 0 < -(a + d) := by omega
  have hTerm2 : 0 < (-(a + d)) * (D * k2) := Int.mul_pos hNegTr hDk
  have hTerm3 : 0 < (D * k2) * (D * k2) := Int.mul_pos hDk hDk
  omega

theorem O13_turing_requires_opposite_diagonal_signs
    (a d Du Dv : Int)
    (hTr : a + d < 0)
    (hDu : 0 < Du) (hDv : 0 < Dv)
    (hTuring : 0 < Dv * a + Du * d) :
    (0 < a ∧ d < 0) ∨ (0 < d ∧ a < 0) := by
  by_cases ha : 0 < a
  · left
    exact ⟨ha, by omega⟩
  · by_cases hd : 0 < d
    · right
      exact ⟨hd, by omega⟩
    · have ha0 : a ≤ 0 := by omega
      have hd0 : d ≤ 0 := by omega
      have h1 : Dv * a ≤ 0 := Int.mul_nonpos_of_nonneg_of_nonpos (Int.le_of_lt hDv) ha0
      have h2 : Du * d ≤ 0 := Int.mul_nonpos_of_nonneg_of_nonpos (Int.le_of_lt hDu) hd0
      omega

theorem O14_turing_requires_faster_inhibitor_diffusion
    (a d Du Dv : Int)
    (hTr : a + d < 0)
    (ha : 0 < a) (hDu : 0 < Du)
    (hTuring : 0 < Dv * a + Du * d) :
    Du < Dv := by
  by_cases hlt : Du < Dv
  · exact hlt
  · have hle : Dv ≤ Du := by omega
    have h1 : Dv * a ≤ Du * a := Int.mul_le_mul_of_nonneg_right hle (Int.le_of_lt ha)
    have h2 : Du * a + Du * d = Du * (a + d) := (Int.mul_add Du a d).symm
    have h3 : Du * (a + d) ≤ 0 := Int.mul_nonpos_of_nonneg_of_nonpos (Int.le_of_lt hDu) (Int.le_of_lt hTr)
    omega

theorem O15_turing_instability_concrete_witness :
    let a : Int := 2; let b : Int := -4; let c : Int := 2; let d : Int := -3
    let Du : Int := 1; let Dv : Int := 10; let k2 : Int := 1
    a + d < 0 ∧
    0 < a * d - b * c ∧
    0 < Dv * a + Du * d ∧
    (a - Du * k2) * (d - Dv * k2) - b * c < 0 := by
  decide

end

end OmegaCellular.TuringFeinbergCRNT
