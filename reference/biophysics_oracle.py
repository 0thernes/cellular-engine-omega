# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2026 Alexander Stephen Donahue
"""Independent Exact Reference Oracle for CELLULAR ENGINE OMEGA v5.2.0 (Build Standard B12).

Zero shared code with C11 (`src/`) or Lean 4 (`lean/`). Uses Python arbitrary-precision
`fractions.Fraction`, `decimal.Decimal` (50-digit precision), exact integers, and standard
library `hashlib.sha256` to cross-verify algebraic, thermodynamic, topological, interaction-net,
hyperplane-projection, substrate, and hybrid-cube invariants.
"""

from __future__ import annotations

from decimal import Decimal, getcontext
from fractions import Fraction
import hashlib
from math import gcd
from typing import Iterable, Sequence

getcontext().prec = 50


def purcell_1dof_path_disp(pts: Sequence[int]) -> int:
    """Exact Stokes displacement along 1-DOF path under canonical antisymmetric connection A(x,y) = y - x."""
    if len(pts) < 2:
        return 0
    return sum(pts[i + 1] - pts[i] for i in range(len(pts) - 1))


def purcell_2dof_loop_area(a: Fraction, b: Fraction, da: Fraction, db: Fraction) -> Fraction:
    """Exact signed Green's theorem area (1/2) * sum (x_i y_{i+1} - x_{i+1} y_i) of 2-DOF rectangular cycle."""
    verts = [(a, b), (a + da, b), (a + da, b + db), (a, b + db)]
    twice = Fraction(0)
    for i in range(4):
        x0, y0 = verts[i]
        x1, y1 = verts[(i + 1) % 4]
        twice += x0 * y1 - x1 * y0
    return twice / 2


def hopfield_ninio_error_rate(p: int, q: int, stages: int) -> Fraction:
    """Exact Hopfield-Ninio kinetic proofreading error rate (p/q)^(stages+1)."""
    if p <= 0 or q <= p or stages < 0:
        raise ValueError("Requires 0 < p < q and stages >= 0")
    return Fraction(p, q) ** (stages + 1)


def eigen_max_genome_length(selective_margin: Fraction, error_rate: Fraction) -> Fraction:
    """Exact Eigen information ceiling L_max = S / mu."""
    if error_rate <= 0:
        raise ValueError("error_rate must be > 0")
    return selective_margin / error_rate


def feinberg_deficiency(n_complexes: int, linkage_classes: int, stoich_dim: int) -> int:
    """Exact Feinberg CRNT deficiency delta = n - l - s."""
    delta = n_complexes - linkage_classes - stoich_dim
    if delta < 0:
        raise ValueError("Deficiency cannot be negative for a valid CRNT network")
    return delta


def turing_mode_trace_and_det(
    a: Fraction, b: Fraction, c: Fraction, d: Fraction, du: Fraction, dv: Fraction, k2: Fraction
) -> tuple[Fraction, Fraction]:
    """Exact trace and determinant of Jacobian J(k^2) = [[a - Du*k^2, b], [c, d - Dv*k^2]]."""
    tr = (a - du * k2) + (d - dv * k2)
    det = (a - du * k2) * (d - dv * k2) - b * c
    return tr, det


def tur_precision_entropy_product(variance: Decimal, entropy_prod: Decimal, mean_current: Decimal) -> Decimal:
    """Exact TUR dimensionless product Q = (Var / J^2) * Sigma (>= 2 at steady state)."""
    if mean_current == 0:
        raise ValueError("mean_current must be non-zero")
    return (variance * entropy_prod) / (mean_current * mean_current)


def poincare_hopf_sphere_sum(charges: Iterable[Fraction]) -> Fraction:
    """Exact sum of topological defect charges on S^2 (must equal 2)."""
    return sum(charges, Fraction(0))


def mota5_motb2_step_degrees() -> tuple[int, int, Fraction]:
    """Exact 5:2 coprime flagellar cogwheel step geometry."""
    g = gcd(5, 2)
    states = (5 * 2) // g
    deg = Fraction(360, states)
    return g, states, deg


def euler_buckling_force(kappa: Fraction, length: Fraction) -> Fraction:
    """Exact Euler buckling critical force F_c = kappa / L^2."""
    if length <= 0:
        raise ValueError("length must be positive")
    return kappa / (length * length)


def odd_elastic_cycle_work(k_odd: Fraction, area: Fraction) -> Fraction:
    """Exact non-reciprocal cyclic work W = 2 * K_o * Area."""
    return 2 * k_odd * area


def project_hyperplane_exact(raw_deltas: Sequence[Fraction]) -> list[Fraction]:
    """Exact O56 conservation-hyperplane projection: x'_i = x_i - (1/N) * sum(x)."""
    n = len(raw_deltas)
    if n == 0:
        return []
    mean_err = sum(raw_deltas, Fraction(0)) / n
    return [x - mean_err for x in raw_deltas]


def bio_inet_replay_exact(
    state: dict[str, int], events: Sequence[tuple[str, int, int]]
) -> dict[str, int]:
    """Independent reference interpreter for the 6-compartment + 2-charge Bio-Frostbite trace."""
    s = dict(state)
    for kind, a, b in events:
        if kind == "tunDryStep":
            s["tunFrozen"] = 1
        elif kind == "tunRehydrate":
            s["tunFrozen"] = 0
        elif s.get("tunFrozen", 0) == 1:
            continue
        elif kind == "dnaToOrg":
            s["dnaMass"] -= a
            s["organelleMass"] += a
            s["rwCount"] += 1
        elif kind == "orgToCell":
            s["organelleMass"] -= a
            s["cellMass"] += a
            s["fuelPool"] -= b
            s["heatPool"] += b
            s["rwCount"] += 1
        elif kind == "cellToCreat":
            s["cellMass"] -= a
            s["creatureMass"] += a
            s["fuelPool"] -= b
            s["heatPool"] += b
            s["rwCount"] += 1
        elif kind == "fuelBurn":
            s["fuelPool"] -= a + b
            s["cellMass"] += a
            s["heatPool"] += b
            s["rwCount"] += 1
        elif kind == "gapExchange":
            s["chargeA"] -= a
            s["chargeB"] += a
            s["rwCount"] += 1
        else:
            raise ValueError(f"Unknown event kind: {kind}")
    return s


def total_mass(s: dict[str, int]) -> int:
    return (
        s["dnaMass"]
        + s["organelleMass"]
        + s["cellMass"]
        + s["creatureMass"]
        + s["fuelPool"]
        + s["heatPool"]
    )


def total_charge(s: dict[str, int]) -> int:
    return s["chargeA"] + s["chargeB"]


def hybrid_cube_expected_counts() -> dict[str, int]:
    """Exact Clean-Slate v3.0 10x10x10 Hybrid Cube theoretical partition & resource caps."""
    return {
        "pm_count": 25,
        "mf_count": 25,
        "gw_count": 25,
        "substrates_total": 75,
        "cube_ok": 661,
        "cube_adapt": 274,
        "cube_over": 20,
        "cube_excl": 45,
        "cube_active_verified": 935,
        "cube_total": 1000,
        "quantum_cap_q": 10,
        "quantum_cap_bytes": 16 * (2**10),
        "b158_trit_states": 3**5,
    }


def sha256_hex(payload: bytes) -> str:
    """Standard NIST FIPS 180-4 SHA-256 hex digest oracle."""
    return hashlib.sha256(payload).hexdigest()
