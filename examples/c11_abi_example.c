/* SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Alexander Stephen Donahue
 * Minimal C11 ABI consumer example demonstrating direct linkage against libomega_cell.
 *
 * Build (Linux):
 *   gcc -O3 -std=c11 -Iinclude examples/c11_abi_example.c bin/libomega_cell.a -lm -o /tmp/c11_abi_example
 */
#include "omega_abi.h"
#include <stdio.h>

int main(void) {
    omega_tb14_result_t r14;
    omega_tb16_result_t r16;
    omega_cube_result_t cube;

    omega_run_tb14(2026001ULL, 0, &r14);
    omega_run_tb16(2026001ULL, 0, &r16);
    omega_run_substrates_and_cube(2026001ULL, &cube);

    printf("CELLULAR ENGINE OMEGA ABI v%s\n", OMEGA_VERSION_STR);
    printf("TB14 Confluence=%d | MassDrift=%lld | ChargeDrift=%lld | Hops=%d\n",
           r14.confluence_match,
           (long long)(r14.mass_after_fifo - r14.mass_before),
           (long long)(r14.charge_after - r14.charge_before),
           r14.causal_hops);
    printf("TB16 ZeroShotR2=%.4f | ProjRes=%.3e | 6TierIntDrift=%lld\n",
           r16.zeroshot_r2, r16.proj_residual, (long long)r16.six_tier_int_drift);
    printf("Substrates=%d/75 | Cube=%d OK / %d ADAPT / %d OVER / %d EXCL | SHA=%s\n",
           cube.total_sub_passed, cube.cube_ok, cube.cube_adapt,
           cube.cube_over, cube.cube_excl, cube.cube_sha256);
    return 0;
}
