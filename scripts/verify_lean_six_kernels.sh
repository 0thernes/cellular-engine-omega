#!/usr/bin/env bash
# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2026 Alexander Stephen Donahue
# Single-source six-kernel Lean 4.35.0 & isolated Challenge/Solution verification script.
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

# Kernel 2 & 3: Replay Solution through leanchecker (--fresh) & paranoid checker
lake env leanchecker --fresh Solution
lake env leanchecker-paranoid --fresh Solution
echo "PASS: leanchecker & leanchecker-paranoid accepted Solution."

# Live negative Lean 4.35.0 kernel rejection test (Playbook v1.3 Item 12):
# prove the active kernel rejects a deliberately false theorem with non-zero exit code.
NEG_FILE="$(mktemp /tmp/omega_negative_kernel_XXXXXX.lean)"
trap 'rm -f "$NEG_FILE" /tmp/omega_ci_axioms.txt /tmp/omega_solution.export /tmp/omega_nanoda.json' EXIT
cat << 'NEGEOF' > "$NEG_FILE"
import OmegaCellularProofs.SubstratesCube
theorem deliberately_false_cube_partition :
    (661 : Nat) + 274 + 20 + 45 = 999 := by
  rfl
NEGEOF
if lake env lean "$NEG_FILE" >/dev/null 2>&1; then
  echo "FAIL: Lean 4.35.0 kernel accepted a deliberately false theorem!" >&2
  exit 1
fi
echo "PASS: Live negative Lean 4.35.0 kernel rejection test rejected false theorem as expected."

# Kernels 4, 5, 6 (direct external replay of ALL 100 comparator.json theorems):
# con-leche (--verified), con-ron (--verified), nanoda_bin (Rust)
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
echo "PASS: con-leche, con-ron, and nanoda_bin independently verified ALL ${#ALL_THMS[@]} exported Solution proofs."

# Sandboxed (bubblewrap) 6-kernel check of all defaultTargets (includes lean4lean!)
lake check --paranoid
echo "PASS: lake check --paranoid verified all defaultTargets across all 6 kernels (incl. lean4lean)."

# Isolated statement-only Challenge vs sorry-free Solution verification across all 6 kernels
lake build Challenge
lake comparator --config comparator.json --paranoid
echo "PASS: All 100 audited theorems verified across ALL 6 independent Lean 4.35.0 kernels (lean, leanchecker-paranoid, lean4lean, nanoda_bin, con-leche, con-ron)!"
