#!/usr/bin/env bash
# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2026 Alexander Stephen Donahue
# Single-source Lean 4.35.0 multi-checker & isolated Challenge/Solution verification script.
# Runs the reference kernel (lean, leanchecker, leanchecker-paranoid) plus four independent
# checkers (lean4lean via lake check --paranoid, nanoda_bin, con-leche --verified, con-ron --verified),
# a negative Lean 4.35.0 elaboration test, live negative axiom-gate tests, and a live
# lake comparator --paranoid sorry-tamper rejection test.
# Invoked identically by GitHub Actions CI (`ci.yml`), `just lean`, and local Linux users.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR/lean"

elan show
lake build
lake env lean VerifyAxioms.lean > /tmp/omega_ci_axioms.txt

# Enforce zero sorryAx, zero native/SAT evaluation, zero Classical.choice, and exact frozen-axiom parity
if grep -E "sorryAx|_native|Lean\.ofReduceBool|Lean\.trustCompiler|Classical\.choice" /tmp/omega_ci_axioms.txt; then
  echo "FAIL: forbidden axiom detected!" >&2
  exit 1
fi
diff -u --strip-trailing-cr ../results/lean4-omega-audit.txt /tmp/omega_ci_axioms.txt
echo "PASS: VerifyAxioms.lean matches frozen results/lean4-omega-audit.txt byte-for-byte (100/100 constructive theorems, 0 Classical.choice)."

# Reference kernel fresh & paranoid replay of Solution
lake env leanchecker --fresh Solution
lake env leanchecker-paranoid --fresh Solution
echo "PASS: Reference kernel checkers (leanchecker, leanchecker-paranoid) accepted Solution."

# Negative Lean 4.35.0 elaboration test (proves a false literal equation fails elaboration):
NEG_FILE="$(mktemp /tmp/omega_negative_elab_XXXXXX.lean)"
trap 'rm -f "$NEG_FILE" /tmp/omega_ci_axioms.txt /tmp/omega_solution.export /tmp/omega_nanoda.json' EXIT
cat << 'NEGEOF' > "$NEG_FILE"
import OmegaCellularProofs.SubstratesCube
theorem deliberately_false_cube_partition :
    (661 : Nat) + 274 + 20 + 45 = 999 := by
  rfl
NEGEOF
if lake env lean "$NEG_FILE" >/dev/null 2>&1; then
  echo "FAIL: Lean 4.35.0 elaboration accepted a deliberately false literal equation!" >&2
  exit 1
fi
echo "PASS: Negative Lean 4.35.0 elaboration test rejected false literal equation as expected."

# Independent export checkers (con-leche --verified, con-ron --verified, nanoda_bin Rust):
# replay the export of ALL 100 comparator.json theorems and their dependencies
mapfile -t ALL_THMS < <(python3 -c 'import json; print("\n".join(json.load(open("comparator.json"))["theorem_names"]))')
echo "Exporting all ${#ALL_THMS[@]} audited theorems from Solution..."
lake env leanexport Solution -- "${ALL_THMS[@]}" > /tmp/omega_solution.export

lake env con-leche --verified /tmp/omega_solution.export
lake env con-ron --verified /tmp/omega_solution.export

cat << 'EOF' > /tmp/omega_nanoda.json
{
  "export_file_path": "/tmp/omega_solution.export",
  "use_stdin": false,
  "permitted_axioms": ["propext", "Quot.sound"],
  "unpermitted_axiom_hard_error": true,
  "nat_extension": true,
  "string_extension": true
}
EOF
lake env nanoda_bin /tmp/omega_nanoda.json
echo "PASS: Independent checkers con-leche, con-ron, and nanoda_bin verified ALL ${#ALL_THMS[@]} exported Solution proofs."

# Sandboxed (bubblewrap) independent lean4lean check of all defaultTargets
lake check --paranoid
echo "PASS: lake check --paranoid verified all defaultTargets (incl. independent lean4lean checker)."

# Isolated statement-only Challenge vs sorry-free Solution comparator verification
lake build Challenge
lake comparator --config comparator.json --paranoid
echo "PASS: lake comparator --paranoid verified Challenge/Solution equivalence for all 100 theorems."

# Live negative axiom-gate tests (sorry, custom axiom, native_decide, Classical.choice)
# + live lake comparator --paranoid sorry-tamper rejection test
python3 "$ROOT_DIR/scripts/test_lean_negative.py" --check-comparator-tamper
echo "PASS: Reference kernel + 4 independent checkers (lean4lean, nanoda_bin, con-leche, con-ron) + negative elaboration & live comparator sorry-tamper rejection tests all PASSED!"
