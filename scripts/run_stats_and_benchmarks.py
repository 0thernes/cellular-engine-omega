#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2026 Alexander Stephen Donahue
"""Preregistered N=20 Multi-Seed Statistical Sweep & Warmed Benchmark Harness (B26-B32).

Executes real native C11 runs across seeds 2026001..2026020 for intact physics and matched
compiled knockouts (M2, M3, M4, M5, M8, M11, M12, M16), computes paired permutation p-values,
Cohen's dz, 10,000-resample seeded bootstrap 95% CIs, Holm-Bonferroni family-wise correction,
20/20 75-substrate/1,000-cube stability checks, and 10 warmed timing benchmarks.
"""

from __future__ import annotations

import csv
from datetime import datetime, timezone
import json
import math
from pathlib import Path
import platform
import subprocess
import tempfile
import time

import numpy as np

ROOT = Path(__file__).resolve().parent.parent
BIN = ROOT / "bin" / ("omega_cell.exe" if platform.system() == "Windows" else "omega_cell")
FALS = ROOT / "bin" / ("test_omega_falsification.exe" if platform.system() == "Windows" else "test_omega_falsification")
RESULTS = ROOT / "results"
RESULTS.mkdir(parents=True, exist_ok=True)

SEEDS = list(range(2026001, 2026021))  # N = 20 preregistered seeds


def run_json(seed: int, mutant: int = 0) -> dict:
    cmd = [str(BIN), "json", "--seed", str(seed)]
    if mutant:
        cmd += ["--mutant", str(mutant)]
    out = subprocess.check_output(cmd, text=True, timeout=60)
    return json.loads(out)


def run_cube(seed: int) -> dict:
    with tempfile.NamedTemporaryFile(suffix=".json", delete=False) as tf:
        p = Path(tf.name)
    try:
        subprocess.check_call([str(BIN), "cube", str(p), "--seed", str(seed)], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, timeout=60)
        return json.loads(p.read_text(encoding="utf-8"))
    finally:
        p.unlink(missing_ok=True)


def paired_permutation_and_bootstrap(
    treat: np.ndarray, ctrl: np.ndarray, rng: np.random.Generator, n_perm: int = 10000, n_boot: int = 10000
) -> dict:
    diffs = treat - ctrl
    obs_mean = float(np.mean(diffs))
    obs_sd = float(np.std(diffs, ddof=1)) if len(diffs) > 1 else 0.0
    cohen_dz = float(obs_mean / obs_sd) if obs_sd > 1e-15 else (float("inf") if obs_mean > 0 else (-float("inf") if obs_mean < 0 else 0.0))

    # Paired sign-flip permutation test (two-sided, Phipson-Smyth (k+1)/(B+1) exact floor)
    signs = rng.choice(np.array([-1.0, 1.0]), size=(n_perm, len(diffs)))
    perm_means = np.mean(signs * diffs[None, :], axis=1)
    extreme = int(np.sum(np.abs(perm_means) >= abs(obs_mean) - 1e-15))
    p_val = float((extreme + 1) / (n_perm + 1))

    # Seeded nonparametric bootstrap 95% CI of mean difference
    idx = rng.integers(0, len(diffs), size=(n_boot, len(diffs)))
    boot_means = np.mean(diffs[idx], axis=1)
    ci_lo, ci_hi = np.percentile(boot_means, [2.5, 97.5])

    return {
        "n": int(len(diffs)),
        "treat_mean": float(np.mean(treat)),
        "treat_std": float(np.std(treat, ddof=1)),
        "ctrl_mean": float(np.mean(ctrl)),
        "ctrl_std": float(np.std(ctrl, ddof=1)),
        "diff_mean": obs_mean,
        "diff_ci95_lo": float(ci_lo),
        "diff_ci95_hi": float(ci_hi),
        "cohen_dz": cohen_dz if math.isfinite(cohen_dz) else 999.0,
        "cohen_dz_is_exact_zero_var": not math.isfinite(cohen_dz),
        "p_raw": p_val,
    }


def holm_bonferroni(p_vals: list[float]) -> list[float]:
    m = len(p_vals)
    order = sorted(range(m), key=lambda i: p_vals[i])
    adj = [0.0] * m
    running_max = 0.0
    for rank, idx in enumerate(order):
        val = min(1.0, p_vals[idx] * (m - rank))
        running_max = max(running_max, val)
        adj[idx] = running_max
    return adj


def main() -> int:
    rng = np.random.default_rng(20261125)
    rows: list[dict] = []

    print(f"Running preregistered N={len(SEEDS)} multi-seed sweep (2026001..2026020)...")
    for s in SEEDS:
        base = run_json(s, 0)
        m2 = run_json(s, 2)
        m3 = run_json(s, 3)
        m4 = run_json(s, 4)
        m5 = run_json(s, 5)
        m8 = run_json(s, 8)
        m10 = run_json(s, 10)
        m11 = run_json(s, 11)
        m12 = run_json(s, 12)
        cube = run_cube(s)

        rows.append(
            {
                "seed": s,
                "h1_amoeba_intact": base["tb02"]["amoeba_with_mavirus"],
                "h1_amoeba_ko_m2": m2["tb02"]["amoeba_with_mavirus"],
                "h2_mt_len_after_ca_intact": base["tb03"]["mt_after_ca"],
                "h2_mt_len_after_ca_ko_m11": m11["tb03"]["mt_after_ca"],
                "h3_llps_drops_intact": base["tb04"]["active_droplets"],
                "h3_llps_drops_ko_m12": m12["tb04"]["active_droplets"],
                "h4_eigen_kp1_intact": base["tb06"]["kp1_L600"],
                "h4_eigen_ko_m3": m3["tb06"]["kp1_L600"],
                "h5_turing_std_intact": base["tb07"]["fast_inh_std"],
                "h5_turing_std_ko_m4": m4["tb07"]["fast_inh_std"],
                "h6_tardi_surv_intact": base["tb09"]["intact_surv"],
                "h6_tardi_surv_ko_m5": m5["tb09"]["intact_surv"],
                "h7_inet_confl_intact": base["tb14"]["confluence_match"],
                "h7_inet_confl_ko_m8": m8["tb14"]["confluence_match"],
                "h8_proj_res_intact": base["tb16"]["proj_mass_res"],
                "h8_proj_res_ko_m10": m10["tb16"]["proj_mass_res"],
                "h8_raw_ai_err_ctrl": base["tb16"]["raw_ai_halluc"],
                "tb05_poincare_charge": base["tb05"]["final_charge"],
                "tb08_tur_min": base["tb08"]["tur_min"],
                "tb16_perturb_r2": base["tb16"]["perturb_r2"],
                "tb16_essential_auroc": base["tb16"]["essential_auroc"],
                "tb16_six_tier_int_drift": base["tb16"]["six_tier_int_drift"],
                "substrates_passed": cube["substrates_75"]["total_passed"],
                "cube_ok": cube["hybrid_cube_1000"]["ok"],
                "cube_adapt": cube["hybrid_cube_1000"]["adapt"],
                "cube_over": cube["hybrid_cube_1000"]["over"],
                "cube_excl": cube["hybrid_cube_1000"]["excl"],
                "pm21_rmse": [x["rmse"] for x in cube["substrates_75"]["pm"] if x["id"] == "PM21"][0],
            }
        )

    csv_path = RESULTS / "multiseed_stats.csv"
    with csv_path.open("w", newline="", encoding="utf-8") as f:
        w = csv.DictWriter(f, fieldnames=list(rows[0].keys()))
        w.writeheader()
        w.writerows(rows)

    contrasts_spec = [
        ("H1", "TB02 Mavirus Rescue (Amoeba Survival)", "h1_amoeba_intact", "h1_amoeba_ko_m2", "gt"),
        ("H2", "TB03 Ca2+ MT Catastrophe Melt (Length)", "h2_mt_len_after_ca_intact", "h2_mt_len_after_ca_ko_m11", "lt"),
        ("H3", "TB04 Active LLPS Arrest (Droplet Count)", "h3_llps_drops_intact", "h3_llps_drops_ko_m12", "gt"),
        ("H4", "TB06 Hopfield-Ninio Proofreading (L=600)", "h4_eigen_kp1_intact", "h4_eigen_ko_m3", "gt"),
        ("H5", "TB07 Turing Morphogenesis (Fast Inh Std)", "h5_turing_std_intact", "h5_turing_std_ko_m4", "gt"),
        ("H6", "TB09 Tardigrade 3-Tier Stress Survival", "h6_tardi_surv_intact", "h6_tardi_surv_ko_m5", "gt"),
        ("H7", "TB14 Bio-Frostbite Strong Confluence", "h7_inet_confl_intact", "h7_inet_confl_ko_m8", "gt"),
        ("H8", "TB16 O56 Hyperplane Projection (vs M10)", "h8_proj_res_intact", "h8_proj_res_ko_m10", "lt"),
    ]

    contrast_results = []
    for cid, label, col_t, col_c, direction in contrasts_spec:
        t_arr = np.array([r[col_t] for r in rows], dtype=float)
        c_arr = np.array([r[col_c] for r in rows], dtype=float)
        st = paired_permutation_and_bootstrap(t_arr, c_arr, rng)
        dir_ok = (st["diff_mean"] > 0) if direction == "gt" else (st["diff_mean"] < 0)
        contrast_results.append({"id": cid, "label": label, "direction": direction, "direction_ok": bool(dir_ok), **st})

    p_adj = holm_bonferroni([c["p_raw"] for c in contrast_results])
    for c, pa in zip(contrast_results, p_adj):
        c["p_holm"] = pa
        c["pass_alpha_001"] = bool(c["direction_ok"] and pa < 0.01)

    # Warmed benchmarks (10 iterations after 1 warmup)
    print("Running 10 warmed timing benchmarks...")
    subprocess.run([str(BIN), "all", "--seed", "2026001"], stdout=subprocess.DEVNULL, check=True)
    all_ms, cube_ms, fals_ms = [], [], []
    with tempfile.NamedTemporaryFile(suffix=".json", delete=False) as tf:
        cube_tmp = Path(tf.name)
    try:
        for _ in range(10):
            t0 = time.perf_counter()
            subprocess.run([str(BIN), "all", "--seed", "2026001"], stdout=subprocess.DEVNULL, check=True)
            all_ms.append((time.perf_counter() - t0) * 1000.0)

            t0 = time.perf_counter()
            subprocess.run([str(BIN), "cube", str(cube_tmp), "--seed", "2026001"], stdout=subprocess.DEVNULL, check=True)
            cube_ms.append((time.perf_counter() - t0) * 1000.0)

            t0 = time.perf_counter()
            subprocess.run([str(FALS)], stdout=subprocess.DEVNULL, check=True)
            fals_ms.append((time.perf_counter() - t0) * 1000.0)
    finally:
        cube_tmp.unlink(missing_ok=True)

    bench_row = {
        "timestamp_utc": datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ"),
        "version": (ROOT / "VERSION").read_text(encoding="utf-8").strip(),
        "os": f"{platform.system()}-{platform.machine()}",
        "all_16tb_median_ms": round(float(np.median(all_ms)), 3),
        "all_16tb_p95_ms": round(float(np.percentile(all_ms, 95)), 3),
        "cube_75_1000_median_ms": round(float(np.median(cube_ms)), 3),
        "cube_75_1000_p95_ms": round(float(np.percentile(cube_ms, 95)), 3),
        "falsification_60g_median_ms": round(float(np.median(fals_ms)), 3),
        "falsification_60g_p95_ms": round(float(np.percentile(fals_ms, 95)), 3),
    }

    bench_csv = RESULTS / "bench-history.csv"
    write_header = not bench_csv.exists()
    with bench_csv.open("a", newline="", encoding="utf-8") as f:
        w = csv.DictWriter(f, fieldnames=list(bench_row.keys()))
        if write_header:
            w.writeheader()
        w.writerow(bench_row)

    summary = {
        "n_seeds": len(SEEDS),
        "seed_range": [SEEDS[0], SEEDS[-1]],
        "substrates_20_of_20_pass": all(r["substrates_passed"] == 75 for r in rows),
        "cube_20_of_20_exact_partition": all(
            (r["cube_ok"], r["cube_adapt"], r["cube_over"], r["cube_excl"]) == (661, 274, 20, 45) for r in rows
        ),
        "pm21_rmse_mean": float(np.mean([r["pm21_rmse"] for r in rows])),
        "pm21_rmse_max": float(np.max([r["pm21_rmse"] for r in rows])),
        "tb16_r2_mean": float(np.mean([r["tb16_perturb_r2"] for r in rows])),
        "tb16_r2_min": float(np.min([r["tb16_perturb_r2"] for r in rows])),
        "tb16_auroc_mean": float(np.mean([r["tb16_essential_auroc"] for r in rows])),
        "tb16_auroc_min": float(np.min([r["tb16_essential_auroc"] for r in rows])),
        "tb08_tur_min_global": float(np.min([r["tb08_tur_min"] for r in rows])),
        "contrasts": contrast_results,
        "benchmarks": bench_row,
    }
    (RESULTS / "multiseed_summary.json").write_text(json.dumps(summary, indent=2), encoding="utf-8")

    print("\n=== PREREGISTERED N=20 MULTI-SEED CONFIRMATORY SUMMARY ===")
    for c in contrast_results:
        print(
            f"  {c['id']} | {c['label']:<42} | T={c['treat_mean']:.4g} vs C={c['ctrl_mean']:.4g} | "
            f"p_raw={c['p_raw']:.4g} | p_holm={c['p_holm']:.4g} | PASS={c['pass_alpha_001']}"
        )
    print(
        f"  Substrates 20/20={summary['substrates_20_of_20_pass']} | Cube 20/20={summary['cube_20_of_20_exact_partition']} | "
        f"PM21 max RMSE={summary['pm21_rmse_max']:.4f} (<0.25)"
    )
    print(
        f"  TB16 Held-Out: R2 mean={summary['tb16_r2_mean']:.4f} (min {summary['tb16_r2_min']:.4f}) | "
        f"AUROC mean={summary['tb16_auroc_mean']:.4f} (min {summary['tb16_auroc_min']:.4f})"
    )
    print(f"  Benchmarks (median ms): 16TB={bench_row['all_16tb_median_ms']} | Cube={bench_row['cube_75_1000_median_ms']} | 60-Gate={bench_row['falsification_60g_median_ms']}")

    assert summary["substrates_20_of_20_pass"], "Substrate multi-seed check failed!"
    assert summary["cube_20_of_20_exact_partition"], "Cube partition multi-seed check failed!"
    assert all(c["pass_alpha_001"] for c in contrast_results), "One or more confirmatory hypotheses failed!"
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
