/-
  CELLULAR ENGINE OMEGA v2.0 — Pure Lean 4 Formal Verification (Zero Mathlib)
  Module 3: Feinberg CRNT Deficiency & Turing Instability Theorems (O11–O15)
-/

namespace OmegaCellular.TuringFeinbergCRNT

/-- Feinberg Network Deficiency `δ = n - l - s` (complexes minus linkage classes minus rank). -/
def feinbergDeficiency (n l s : Nat) : Nat := n - l - s

/-- O11 (Feinberg Network Deficiency Non-Negativity & Zero Characterization):
    Under the incidence-graph rank bound `l + s ≤ n`, `δ = n - l - s` is exact and
    `δ = 0 ↔ n = l + s` (the exact structural gate of Feinberg's Deficiency Zero Theorem). -/
theorem O11_feinberg_deficiency_nonneg (n l s : Nat) (hBound : l + s ≤ n) :
    feinbergDeficiency n l s + l + s = n ∧ (feinbergDeficiency n l s = 0 ↔ n = l + s) := by
  unfold feinbergDeficiency
  omega

/-- O12 (Turing Equal-Diffusion Impossibility Theorem):
    For ANY 2×2 locally stable reaction Jacobian (`a + d < 0` and `a * d - b * c > 0`),
    equal diffusion `Du = Dv = D > 0` at ANY nonzero spatial wavenumber `k2 > 0` yields:
    (1) strictly negative diffused trace `(a - D*k2) + (d - D*k2) < 0`, AND
    (2) strictly positive diffused determinant `(a - D*k2)*(d - D*k2) - b*c > 0`.
    Hence equal diffusion can NEVER produce a Turing pattern instability! -/
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

/-- O13 (Turing Activator–Inhibitor Opposite-Sign Necessity):
    Under local stability `a + d < 0` and positive diffusivities `Du > 0, Dv > 0`,
    Turing's condition `Dv * a + Du * d > 0` strictly forces opposite diagonal signs:
    either (`a > 0 ∧ d < 0`) or (`d > 0 ∧ a < 0`). -/
theorem O13_turing_requires_opposite_diagonal_signs
    (a d Du Dv : Int)
    (hTr : a + d < 0)
    (hDu : 0 < Du) (hDv : 0 < Dv)
    (hTuring : 0 < Dv * a + Du * d) :
    (0 < a ∧ d < 0) ∨ (0 < d ∧ a < 0) := by
  by_cases ha : 0 < a
  · left
    refine ⟨ha, ?_⟩
    omega
  · by_cases hd : 0 < d
    · right
      refine ⟨hd, ?_⟩
      omega
    · have ha0 : a ≤ 0 := by omega
      have hd0 : d ≤ 0 := by omega
      have h1 : Dv * a ≤ 0 := Int.mul_nonpos_of_nonneg_of_nonpos (Int.le_of_lt hDv) ha0
      have h2 : Du * d ≤ 0 := Int.mul_nonpos_of_nonneg_of_nonpos (Int.le_of_lt hDu) hd0
      omega

/-- O14 (Turing Faster-Inhibitor Diffusion Necessity: `D_act < D_inh`):
    When `u` is the local autocatalytic activator (`a > 0`), `v` is the self-damping
    inhibitor (`d < 0`), local stability `a + d < 0` holds, and Turing's instability
    condition `Dv * a + Du * d > 0` holds, then strictly `Du < Dv`. -/
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

/-- O15 (Concrete Turing Instability Dispersion Witness):
    Exact Jacobian `a = 2, b = -4, c = 2, d = -3` with `Du = 1, Dv = 10` at wavenumber
    `k2 = 1` is locally stable (`Tr = -1 < 0, Det = +2 > 0`) yet diffuses into a negative
    determinant (`Det(k=1) = -5 < 0`), proving a strictly positive unstable Turing eigenmode! -/
theorem O15_turing_instability_concrete_witness :
    let a : Int := 2; let b : Int := -4; let c : Int := 2; let d : Int := -3
    let Du : Int := 1; let Dv : Int := 10; let k2 : Int := 1
    a + d < 0 ∧
    0 < a * d - b * c ∧
    0 < Dv * a + Du * d ∧
    (a - Du * k2) * (d - Dv * k2) - b * c < 0 := by
  decide

end OmegaCellular.TuringFeinbergCRNT
