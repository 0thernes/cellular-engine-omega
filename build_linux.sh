#!/usr/bin/env bash
set -euo pipefail

# SPDX-License-Identifier: Apache-2.0
# CELLULAR ENGINE OMEGA v5.4.0 — Native Linux ELF64 Zero-Dependency Build & Verification
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CC="${CC:-gcc}"
mkdir -p "$SCRIPT_DIR/bin" "$SCRIPT_DIR/results"

CORE=(
  "$SCRIPT_DIR/src/omega_rng_hash.c"
  "$SCRIPT_DIR/src/exp_01_06.c"
  "$SCRIPT_DIR/src/exp_07_12.c"
  "$SCRIPT_DIR/src/exp_13_open_proliferation.c"
  "$SCRIPT_DIR/src/exp_14_bio_inet.c"
  "$SCRIPT_DIR/src/exp_15_deep_strata.c"
  "$SCRIPT_DIR/src/exp_16_virtual_cell_ai.c"
  "$SCRIPT_DIR/src/omega_substrates_cube.c"
)

CFLAGS=(-O3 -std=c11 -Wall -Wextra -Wshadow -Werror -Wno-unused-parameter -ffp-contract=off "-I$SCRIPT_DIR/include")
LDFLAGS=(-lm)

echo "[1/5] Building bin/omega_cell (Linux ELF64 via $CC) ..."
"$CC" "${CFLAGS[@]}" "${CORE[@]}" "$SCRIPT_DIR/src/omega_main_cli.c" -o "$SCRIPT_DIR/bin/omega_cell" "${LDFLAGS[@]}"

echo "[2/5] Building bin/libomega_cell.so ..."
"$CC" "${CFLAGS[@]}" -fPIC -shared "${CORE[@]}" -o "$SCRIPT_DIR/bin/libomega_cell.so" "${LDFLAGS[@]}"

echo "[3/5] Building bin/test_omega_falsification ..."
"$CC" "${CFLAGS[@]}" "${CORE[@]}" "$SCRIPT_DIR/test/test_omega_falsification.c" -o "$SCRIPT_DIR/bin/test_omega_falsification" "${LDFLAGS[@]}"

echo "[4/5] Building bin/fuzz_omega_standalone ..."
"$CC" "${CFLAGS[@]}" "$SCRIPT_DIR/src/omega_rng_hash.c" "$SCRIPT_DIR/src/exp_07_12.c" "$SCRIPT_DIR/src/exp_14_bio_inet.c" "$SCRIPT_DIR/test/fuzz_omega_cli_codec.c" -o "$SCRIPT_DIR/bin/fuzz_omega_standalone" "${LDFLAGS[@]}"

echo "[5/5] Building static archive bin/libomega_cell.a ..."
OBJS=()
for src in "${CORE[@]}"; do
  obj="$SCRIPT_DIR/bin/$(basename "${src%.c}.o")"
  "$CC" -c "${CFLAGS[@]}" -fPIC "$src" -o "$obj"
  OBJS+=("$obj")
done
ar rcs "$SCRIPT_DIR/bin/libomega_cell.a" "${OBJS[@]}"
rm -f "${OBJS[@]}"

if [ "${OMEGA_SANITIZE:-0}" = "1" ]; then
  echo ""
  echo "Building & running ASan+UBSan falsification + fuzz binary (-fsanitize=address,undefined)..."
  "$CC" -O1 -g -std=c11 -Wall -Wextra -Wshadow -Werror -Wno-unused-parameter -fsanitize=address,undefined -fno-omit-frame-pointer "-I$SCRIPT_DIR/include" "${CORE[@]}" "$SCRIPT_DIR/test/test_omega_falsification.c" -o "$SCRIPT_DIR/bin/test_omega_asan" "${LDFLAGS[@]}"
  "$SCRIPT_DIR/bin/test_omega_asan"
  "$CC" -O1 -g -std=c11 -Wall -Wextra -Wshadow -Werror -Wno-unused-parameter -fsanitize=address,undefined -fno-omit-frame-pointer "-I$SCRIPT_DIR/include" "$SCRIPT_DIR/src/omega_rng_hash.c" "$SCRIPT_DIR/src/exp_07_12.c" "$SCRIPT_DIR/src/exp_14_bio_inet.c" "$SCRIPT_DIR/test/fuzz_omega_cli_codec.c" -o "$SCRIPT_DIR/bin/fuzz_omega_asan" "${LDFLAGS[@]}"
  "$SCRIPT_DIR/bin/fuzz_omega_asan"
fi

if command -v cppcheck >/dev/null 2>&1; then
  echo ""
  echo "Running report-only static analysis via cppcheck..."
  cppcheck --enable=warning,performance,portability --std=c11 -I"$SCRIPT_DIR/include" "$SCRIPT_DIR/src" "$SCRIPT_DIR/test" || true
fi

echo ""
echo "Running 60-Gate Falsification Suite + 16/16 Planted Mutants + 10-Seed Sweep + 75-Sub/1,000-Cube..."
"$SCRIPT_DIR/bin/test_omega_falsification" | tee "$SCRIPT_DIR/results/falsification_linux.txt"

echo ""
echo "Running Standalone Fuzz + Property-Based Harness..."
"$SCRIPT_DIR/bin/fuzz_omega_standalone"

echo ""
echo "Running 16-Testbed Suite (Seed 2026001)..."
"$SCRIPT_DIR/bin/omega_cell" all --seed 2026001 | tee "$SCRIPT_DIR/results/testbeds_all_linux.txt"

echo ""
echo "Running Clean-Slate v3.0 75 Substrates + 1,000-Cell Hybrid Cube (Seed 2026001)..."
"$SCRIPT_DIR/bin/omega_cell" cube "$SCRIPT_DIR/results/omega_75_substrates_1000_cube.json" --seed 2026001

echo ""
echo "Exporting Live C11 Runtime Trace & Cube Receipt -> lean/OmegaCellularProofs/LiveC11Receipt.lean ..."
"$SCRIPT_DIR/bin/omega_cell" --export-lean-trace "$SCRIPT_DIR/lean/OmegaCellularProofs/LiveC11Receipt.lean" --seed 2026001

if command -v node >/dev/null 2>&1; then
  echo ""
  echo "Verifying JSON serialization schema via Node.js..."
  "$SCRIPT_DIR/bin/omega_cell" json --seed 2026001 | node -e '
    const fs = require("fs");
    const j = JSON.parse(fs.readFileSync(0, "utf8"));
    if (!j.version || !/^\d+\.\d+\.\d+$/.test(j.version)) {
      throw new Error("Missing or invalid top-level version field");
    }
    if (!j.master_sha256 || !/^[0-9a-f]{64}$/.test(j.master_sha256)) {
      throw new Error("Missing or invalid top-level master_sha256 field");
    }
    for (let i = 1; i <= 16; i++) {
      const k = "tb" + String(i).padStart(2, "0");
      if (!j[k] || !/^[0-9a-f]{64}$/.test(j[k].sha256)) {
        throw new Error("Missing or invalid SHA-256 for " + k);
      }
    }
    if (j.tb14.causal_hops !== 8) throw new Error("Expected 8 causal 4-layer hops");
    if (j.tb16.syn3a_total !== 493 || j.tb16.syn3a_dark !== 92) throw new Error("Unexpected syn3A genome counts");
    if (j.tb16.six_tier_int_drift !== 0) throw new Error("Expected 0 six_tier_int_drift");
    console.log("PASS: Validated v" + j.version + " master_sha256=" + j.master_sha256.slice(0, 12) + "... and all 16 JSON testbeds.");
  '
fi

if command -v lake >/dev/null 2>&1; then
  echo ""
  echo "Verifying 100 Constructive Pure Lean 4.35.0 Proofs (Palomar Challenge/Solution + Strict Constructive Axiom Allowlist)..."
  (
    cd "$SCRIPT_DIR/lean"
    lake build 2>&1 | tee "$SCRIPT_DIR/results/lean4-build-linux.log"
    lake build Challenge 2>&1 | tee "$SCRIPT_DIR/results/lean4-challenge-linux.log"
    lake env lean VerifyAxioms.lean 2>&1 | tee "$SCRIPT_DIR/results/lean4-omega-audit-linux.txt"
    lake env leanchecker --fresh Solution
    lake env leanchecker-paranoid --fresh Solution
  )
  python3 - "$SCRIPT_DIR/results/lean4-build-linux.log" "$SCRIPT_DIR/results/lean4-omega-audit-linux.txt" <<'PY'
import re, sys
build_log = open(sys.argv[1], encoding="utf-8").read()
axiom_log = open(sys.argv[2], encoding="utf-8").read()
combined = build_log + "\n" + axiom_log
if re.search(r"sorryAx|declaration uses 'sorry'|warning:|Lean\.ofReduceBool|Lean\.trustCompiler|_native\.bv_decide|Classical\.choice", combined):
    print("ERROR: sorryAx, Classical.choice, unauthorized compiler/SAT axiom, or warning detected!", file=sys.stderr)
    sys.exit(1)
allowed = {"propext", "Quot.sound"}
seen = set()
for m in re.finditer(r"'(OmegaCellular\.[^']+)' (depends on axioms:\s*\[([^\]]+)\]|does not depend on any axioms)", axiom_log, re.S):
    seen.add(m.group(1))
    if m.group(3):
        axs = {a.strip() for a in m.group(3).split(",") if a.strip()}
        bad = axs - allowed
        if bad:
            print(f"ERROR: Unauthorized axiom(s) {bad} in {m.group(1)}", file=sys.stderr)
            sys.exit(1)
if len(seen) < 100:
    print(f"ERROR: Expected >= 100 unique audited theorems, found {len(seen)}", file=sys.stderr)
    sys.exit(1)
if "OmegaCellular.VerifyTraceReceipt.O63_live_c11_receipt_verified" not in seen:
    print("ERROR: Missing O63_live_c11_receipt_verified in axiom audit!", file=sys.stderr)
    sys.exit(1)
if "OmegaCellular.SubstratesCube.O72_live_c11_cube_receipt_verified" not in seen:
    print("ERROR: Missing O72_live_c11_cube_receipt_verified in axiom audit!", file=sys.stderr)
    sys.exit(1)
if "OmegaCellular.AdversarialTests.O100_refute_hybrid_cube_partition_drift" not in seen:
    print("ERROR: Missing O100_refute_hybrid_cube_partition_drift in axiom audit!", file=sys.stderr)
    sys.exit(1)
print(f"PASS: All {len(seen)}/100 constructive OmegaCellularProofs theorems verified (0 Classical.choice, 0 sorryAx, 0 custom axioms, 0 warnings).")
PY
fi

echo ""
echo "CELLULAR ENGINE OMEGA v5.4.0 LINUX ELF64 BUILD & VERIFICATION COMPLETE."
