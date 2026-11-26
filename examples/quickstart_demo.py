#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2026 Alexander Stephen Donahue
"""Quickstart example: invoke native omega_cell JSON API and cross-check invariants in Python."""

from __future__ import annotations

import json
from pathlib import Path
import platform
import subprocess
import sys

ROOT = Path(__file__).resolve().parent.parent
BIN = ROOT / "bin" / ("omega_cell.exe" if platform.system() == "Windows" else "omega_cell")


def main() -> int:
    if not BIN.exists():
        print(f"Binary not found at {BIN}. Run build.ps1 (Windows) or ./build_linux.sh (Linux) first.", file=sys.stderr)
        return 1
    version = (ROOT / "VERSION").read_text(encoding="utf-8").strip()
    proc = subprocess.run([str(BIN), "json", "--seed", "2026001"], capture_output=True, text=True, check=True)
    data = json.loads(proc.stdout)
    print(f"CELLULAR ENGINE OMEGA v{data['version']} (VERSION={version}) | Seed={data['seed']} | Elapsed={data['elapsed_ms']:.2f} ms")
    print(f"Master SHA-256 : {data['master_sha256']}")
    print(f"TB01 Scallop   : net_disp={data['tb01']['scallop_net_disp']:.3e} | SHA={data['tb01']['sha256'][:16]}...")
    print(f"TB05 Sphere    : defect_sum={data['tb05']['final_charge']:.6f} (Poincare-Hopf +2)")
    print(f"TB14 Bio-INet  : confluence={data['tb14']['confluence_match']} | mass_drift={data['tb14']['mass_drift_fifo']} | hops={data['tb14']['causal_hops']}")
    print(f"TB16 VirtCell  : zero_shot_R2={data['tb16']['perturb_r2']:.4f} | proj_res={data['tb16']['proj_mass_res']:.3e} | syn3A={data['tb16']['syn3a_total']}/{data['tb16']['syn3a_dark']}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
