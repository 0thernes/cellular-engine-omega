#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2026 Alexander Stephen Donahue
"""Live Negative Lean 4.35.0 Axiom-Gate & Comparator Tamper Rejection Test (Build Standard B06 / Playbook v1.3).

Verifies that the live Lean 4.35.0 verification gates genuinely CATCH and REJECT:
  1. A planted `sorry` (`sorryAx`)
  2. A planted custom `axiom` (`bad_custom_ax`)
  3. A planted `native_decide` (`_native` / `ofReduceBool`)
  4. A planted `Classical.choice` (`Classical.em`) — enforcing the strict constructive allowlist
  5. (Optional `--check-comparator-tamper` on Linux / bwrap) A live `sorry`-tampered `Solution`
     fed into `lake comparator --config comparator.json --paranoid`, asserting non-zero rejection.

Usage:
  python3 scripts/test_lean_negative.py
  python3 scripts/test_lean_negative.py --check-comparator-tamper
"""

from __future__ import annotations

import argparse
from pathlib import Path
import platform
import re
import shutil
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parent.parent
LEAN_DIR = ROOT / "lean"
ALLOWED = {"propext", "Quot.sound"}
FORBIDDEN_RE = re.compile(r"(sorryAx|ofReduceBool|trustCompiler|_native|Classical\.choice)", re.I)

CASES = [
    (
        "planted_sorry",
        "theorem bad_sorry : (1 : Nat) = 2 := by sorry\n#print axioms bad_sorry\n",
        "sorryAx",
    ),
    (
        "planted_custom_axiom",
        "axiom bad_custom_ax : (1 : Nat) = 2\ntheorem bad_ax_thm : (1 : Nat) = 2 := bad_custom_ax\n#print axioms bad_ax_thm\n",
        "bad_custom_ax",
    ),
    (
        "planted_native_decide",
        "theorem bad_native : (1 : Nat) + 1 = 2 := by native_decide\n#print axioms bad_native\n",
        "_native",
    ),
    (
        "planted_classical_choice",
        "theorem bad_classical (P : Prop) : P ∨ ¬P := Classical.em P\n#print axioms bad_classical\n",
        "Classical.choice",
    ),
]


def find_lake() -> str | None:
    lake = shutil.which("lake")
    if lake:
        return lake
    exe = "lake.exe" if platform.system() == "Windows" else "lake"
    for cand in (
        Path.home() / ".elan" / "bin" / exe,
        Path.home() / ".elan" / "toolchains" / "leanprover--lean4---v4.35.0" / "bin" / exe,
    ):
        if cand.exists():
            return str(cand)
    return None


def audit_accepts(output: str) -> tuple[bool, str]:
    if FORBIDDEN_RE.search(output):
        return False, "forbidden axiom token detected"
    matches = re.findall(
        r"'([^']+)'\s+(does not depend on any axioms|depends on axioms:\s*\[([^\]]*)\])",
        output,
    )
    if not matches:
        return False, "no #print axioms declaration produced"
    for name, _, axs in matches:
        for ax in [a.strip() for a in axs.split(",") if a.strip()]:
            if ax not in ALLOWED:
                return False, f"unpermitted axiom '{ax}' in {name}"
    return True, "accepted"


def run_comparator_tamper_test(lake: str) -> None:
    """Copy lean/ to a temp directory, inject `sorry` into O100, rebuild, and assert comparator rejects."""
    with tempfile.TemporaryDirectory(prefix="omega_comp_tamper_") as tmp:
        tamper_dir = Path(tmp) / "lean"
        shutil.copytree(LEAN_DIR, tamper_dir)
        adv_path = tamper_dir / "OmegaCellularProofs" / "AdversarialTests.lean"
        orig = adv_path.read_text(encoding="utf-8")
        target = (
            "theorem O100_refute_hybrid_cube_partition_drift :\n"
            "    ¬ ((661 : Nat) + 274 + 20 + 45 ≠ 1000) := by\n"
            "  decide"
        )
        tampered = (
            "theorem O100_refute_hybrid_cube_partition_drift :\n"
            "    ¬ ((661 : Nat) + 274 + 20 + 45 ≠ 1000) := by\n"
            "  sorry"
        )
        if target not in orig:
            print("FAIL: Could not locate O100_refute_hybrid_cube_partition_drift to inject sorry tamper!", file=sys.stderr)
            sys.exit(1)
        adv_path.write_text(orig.replace(target, tampered, 1), encoding="utf-8", newline="\n")

        bproc = subprocess.run([lake, "build"], cwd=tamper_dir, capture_output=True, text=True, timeout=180)
        if bproc.returncode != 0:
            print(f"FAIL: Tampered Solution failed to build before comparator check:\n{bproc.stdout}\n{bproc.stderr}", file=sys.stderr)
            sys.exit(1)

        cproc = subprocess.run(
            [lake, "comparator", "--config", "comparator.json", "--paranoid"],
            cwd=tamper_dir,
            capture_output=True,
            text=True,
            timeout=180,
        )
        if cproc.returncode == 0:
            print(
                f"FAIL: lake comparator --paranoid WRONGLY ACCEPTED a sorry-tampered Solution!\n{cproc.stdout}\n{cproc.stderr}",
                file=sys.stderr,
            )
            sys.exit(1)
        print("  [CAUGHT] comparator_sorry_tamper  -> rejected by lake comparator --paranoid (exit != 0)")


def main() -> None:
    parser = argparse.ArgumentParser(description="Live negative Lean 4.35.0 axiom-gate & comparator tamper audit.")
    parser.add_argument("--allow-no-lake", action="store_true", help="Exit 0 with SKIP notice if lake is unavailable")
    parser.add_argument(
        "--check-comparator-tamper",
        action="store_true",
        help="Also run live lake comparator --paranoid sorry-tamper rejection test (requires Linux bwrap).",
    )
    args = parser.parse_args()

    lake = find_lake()
    if not lake:
        if args.allow_no_lake:
            print("SKIP: lake binary not found on PATH (--allow-no-lake).")
            return
        print("FAIL: lake binary not found on PATH.", file=sys.stderr)
        sys.exit(1)

    with tempfile.TemporaryDirectory(prefix="omega_lean_neg_") as tmp:
        tmp_dir = Path(tmp)
        for name, code, expected_marker in CASES:
            src = tmp_dir / f"{name}.lean"
            src.write_text(code, encoding="utf-8", newline="\n")
            proc = subprocess.run(
                [lake, "env", "lean", str(src)],
                cwd=LEAN_DIR,
                capture_output=True,
                text=True,
                timeout=60,
            )
            combined = (proc.stdout or "") + "\n" + (proc.stderr or "")
            accepted, reason = audit_accepts(combined)
            if accepted:
                print(f"FAIL: negative case '{name}' was WRONGLY ACCEPTED!\n{combined}", file=sys.stderr)
                sys.exit(1)
            if expected_marker not in combined and proc.returncode == 0:
                print(
                    f"FAIL: negative case '{name}' did not emit expected marker '{expected_marker}':\n{combined}",
                    file=sys.stderr,
                )
                sys.exit(1)
            print(f"  [CAUGHT] {name:<24} -> rejected ({reason})")

    if args.check_comparator_tamper:
        run_comparator_tamper_test(lake)
        print(
            "PASS: All 4/4 live negative Lean 4.35.0 axiom exploits (sorry, custom axiom, native_decide, Classical.choice) "
            "AND live lake comparator sorry-tamper test were rejected!"
        )
    else:
        print(
            "PASS: All 4/4 live negative Lean 4.35.0 axiom exploits (sorry, custom axiom, native_decide, Classical.choice) were rejected!"
        )


if __name__ == "__main__":
    main()
