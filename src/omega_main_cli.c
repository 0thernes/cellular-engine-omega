#include "omega_bare_crt.h"
#include "omega_abi.h"

static inline int64_t i64_abs_cli(int64_t v) { return (v < 0) ? -v : v; }

static void make_demo_autophage_dna(uint64_t seed, uint8_t dna[256]) {
    OmegaRng rng; omega_rng_seed(&rng, seed ^ 0xA070F46EULL);
    for (int i = 0; i < 256; ++i) dna[i] = (uint8_t)(omega_rng_u32(&rng) & 0xFFu);
}

static uint64_t cli_feral_mix(uint64_t base_seed) {
    double t = omega_time_sec();
    uint64_t bits = 0;
    memcpy(&bits, &t, sizeof(bits));
#if defined(__x86_64__) || defined(_M_X64)
    uint32_t lo = 0, hi = 0;
    __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
    uint64_t tsc = ((uint64_t)hi << 32) | (uint64_t)lo;
    return base_seed ^ bits ^ (tsc * 0x9E3779B97F4A7C15ULL);
#else
    return base_seed ^ (bits * 0x9E3779B97F4A7C15ULL);
#endif
}

static void build_tier_ledger(const TB04Result* r4, const TB05Result* r5,
                              const TB13Result* r13, const TB14Result* r14,
                              const TB15Result* r15, OmegaTierLedger* out_ledger) {
    out_ledger->t1_raf_mass_residual       = r15->raf_mass_residual;
    out_ledger->t1_raf_total_mass          = 128.0;
    out_ledger->t2_three_genome_mass_drift = r15->three_genome_mass_drift;
    out_ledger->t3_llps_mass_drift         = r4->mass_drift_eq;
    out_ledger->t3_llps_total_mass         = 1024.0;
    out_ledger->t4_inet_mass_drift         = i64_abs_cli(r14->mass_drift_fifo) +
                                             i64_abs_cli(r14->mass_drift_lifo) +
                                             i64_abs_cli(r14->mass_drift_random);
    out_ledger->t4_inet_charge_drift       = i64_abs_cli(r14->gap_charge_drift);
    out_ledger->t5_biosphere_mass_residual = r13->global_mass_residual;
    out_ledger->t5_biosphere_total_mass    = 7920.0;
    out_ledger->t5_topology_charge_err     = r5->max_charge_error;
}

static void print_float_array(const float* arr, int n) {
    putchar('[');
    for (int i = 0; i < n; ++i) {
        if (i) putchar(',');
        printf("%.3f", (double)arr[i]);
    }
    putchar(']');
}

static void print_double_array(const double* arr, int n) {
    putchar('[');
    for (int i = 0; i < n; ++i) {
        if (i) putchar(',');
        printf("%.5f", arr[i]);
    }
    putchar(']');
}

static void print_u8_array(const uint8_t* arr, int n) {
    putchar('[');
    for (int i = 0; i < n; ++i) {
        if (i) putchar(',');
        printf("%u", (unsigned)arr[i]);
    }
    putchar(']');
}

static int export_lean_trace(const char* out_path, uint64_t seed) {
    TB04Result r4;
    TB05Result r5;
    TB13Result r13;
    TB14Result r14;
    TB15Result r15;
    TB16Result r16;
    OmegaTierLedger ledger;
    OmegaSubstratesCubeResult sc;

    omega_run_tb04(seed, 0, &r4);
    omega_run_tb05(seed, 0, &r5);
    omega_run_tb13(seed, 0, 0, &r13);
    omega_run_tb14(seed, 0, 0, &r14);
    omega_run_tb15(seed, 0, &r15);
    build_tier_ledger(&r4, &r5, &r13, &r14, &r15, &ledger);
    omega_run_tb16_coupled(seed, 0, &ledger, &r16);
    omega_run_substrates_75_and_cube_1000(seed, &sc);

    char concat[2 * 64 + 1];
    memcpy(concat,      r14.sha256, 64);
    memcpy(concat + 64, r16.sha256, 64);
    concat[128] = '\0';
    char receipt_sha[65];
    omega_sha256_hex(concat, 128, receipt_sha);

    int64_t mass_before     = r14.init_mass_q;
    int64_t mass_after_fifo = r14.init_mass_q + r14.mass_drift_fifo;
    int64_t mass_after_lifo = r14.init_mass_q + r14.mass_drift_lifo;
    int64_t mass_after_rand = r14.init_mass_q + r14.mass_drift_random;
    int64_t charge_before   = r14.init_charge_q;
    int64_t charge_after    = r14.init_charge_q + r14.gap_charge_drift;

    FILE* fp = fopen(out_path, "wb");
    if (!fp) {
        fprintf(stderr, "ERROR: Failed to open %s for writing Lean trace receipt.\n", out_path);
        return 1;
    }

    fprintf(fp, "module\n");
    fprintf(fp, "-- SPDX-License-Identifier: Apache-2.0\n");
    fprintf(fp, "-- Copyright (c) 2026 Alexander Stephen Donahue\n\n");
    fprintf(fp, "/-!\n");
    fprintf(fp, "# LiveC11Receipt.lean — Auto-Generated Live C11 Runtime Trace Receipt (v%s)\n\n", OMEGA_VERSION_STR);
    fprintf(fp, "Emitted directly by `omega_cell --export-lean-trace` executing real C11 `TB14`\n");
    fprintf(fp, "(Bio-Frostbite 4-Layer INet VM) + coupled `TB16` (Virtual-Cell AI & 6-Tier Closure)\n");
    fprintf(fp, "+ Clean-Slate v3.0 75 Substrates & 1,000-Cell Hybrid Cube (`omega_substrates_cube.c`)\n");
    fprintf(fp, "on deterministic Seed `%llu`. Verified inside the Lean 4 kernel by `O63` and `O72`.\n",
            (unsigned long long)seed);
    fprintf(fp, "-/\n\n");
    fprintf(fp, "namespace OmegaCellular.LiveC11Receipt\n\n");
    fprintf(fp, "@[expose] public section\n\n");
    fprintf(fp, "structure LiveReceipt where\n");
    fprintf(fp, "  seed               : Nat\n");
    fprintf(fp, "  nodesInit          : Nat\n");
    fprintf(fp, "  rewritesFifo       : Nat\n");
    fprintf(fp, "  rewritesLifo       : Nat\n");
    fprintf(fp, "  rewritesRandom     : Nat\n");
    fprintf(fp, "  layer1Rewrites     : Nat\n");
    fprintf(fp, "  layer2Rewrites     : Nat\n");
    fprintf(fp, "  layer3Rewrites     : Nat\n");
    fprintf(fp, "  layer4Rewrites     : Nat\n");
    fprintf(fp, "  confluenceMatch    : Nat\n");
    fprintf(fp, "  massBefore         : Int\n");
    fprintf(fp, "  massAfterFifo      : Int\n");
    fprintf(fp, "  massAfterLifo      : Int\n");
    fprintf(fp, "  massAfterRandom    : Int\n");
    fprintf(fp, "  chargeBefore       : Int\n");
    fprintf(fp, "  chargeAfter        : Int\n");
    fprintf(fp, "  wireErrors         : Nat\n");
    fprintf(fp, "  russianDollDepth   : Nat\n");
    fprintf(fp, "  tunDryPairs        : Nat\n");
    fprintf(fp, "  tunDryPower        : Int\n");
    fprintf(fp, "  tunRehydRewrites   : Nat\n");
    fprintf(fp, "  ssinArbiterPairs   : Nat\n");
    fprintf(fp, "  ssinContended      : Nat\n");
    fprintf(fp, "  ssinConfluentRw    : Nat\n");
    fprintf(fp, "  causalHops         : Nat\n");
    fprintf(fp, "  threeGenMassDrift  : Int\n");
    fprintf(fp, "  syn3aTotalGenes    : Nat\n");
    fprintf(fp, "  syn3aDarkGenes     : Nat\n");
    fprintf(fp, "  syn3aDarkDiscov    : Nat\n");
    fprintf(fp, "  sixTierIntDrift    : Int\n");
    fprintf(fp, "  pmPassed           : Nat\n");
    fprintf(fp, "  mfPassed           : Nat\n");
    fprintf(fp, "  gwPassed           : Nat\n");
    fprintf(fp, "  cubeOk             : Nat\n");
    fprintf(fp, "  cubeAdapt          : Nat\n");
    fprintf(fp, "  cubeOver           : Nat\n");
    fprintf(fp, "  cubeExcl           : Nat\n");
    fprintf(fp, "  cubeActiveVerif    : Nat\n");
    fprintf(fp, "  cubeVerifiedTot    : Nat\n");
    fprintf(fp, "  quantumCapQ        : Nat\n");
    fprintf(fp, "  quantumCapBytes    : Nat\n");
    fprintf(fp, "  b158TritStates     : Nat\n");
    fprintf(fp, "  shaFifo            : String\n");
    fprintf(fp, "  shaLifo            : String\n");
    fprintf(fp, "  shaRandom          : String\n");
    fprintf(fp, "  tb14Sha256         : String\n");
    fprintf(fp, "  tb16Sha256         : String\n");
    fprintf(fp, "  cubeSha256         : String\n");
    fprintf(fp, "  receiptSha256      : String\n");
    fprintf(fp, "  deriving Repr, DecidableEq\n\n");
    fprintf(fp, "def liveReceipt : LiveReceipt :=\n");
    fprintf(fp, "  { seed              := %llu\n", (unsigned long long)seed);
    fprintf(fp, "  , nodesInit         := %d\n",   r14.total_nodes_init);
    fprintf(fp, "  , rewritesFifo      := %d\n",   r14.rewrites_fifo);
    fprintf(fp, "  , rewritesLifo      := %d\n",   r14.rewrites_lifo);
    fprintf(fp, "  , rewritesRandom    := %d\n",   r14.rewrites_random);
    fprintf(fp, "  , layer1Rewrites    := %d\n",   r14.layer_rewrites[0]);
    fprintf(fp, "  , layer2Rewrites    := %d\n",   r14.layer_rewrites[1]);
    fprintf(fp, "  , layer3Rewrites    := %d\n",   r14.layer_rewrites[2]);
    fprintf(fp, "  , layer4Rewrites    := %d\n",   r14.layer_rewrites[3]);
    fprintf(fp, "  , confluenceMatch   := %d\n",   r14.confluence_exact_match);
    fprintf(fp, "  , massBefore        := %lld\n", (long long)mass_before);
    fprintf(fp, "  , massAfterFifo     := %lld\n", (long long)mass_after_fifo);
    fprintf(fp, "  , massAfterLifo     := %lld\n", (long long)mass_after_lifo);
    fprintf(fp, "  , massAfterRandom   := %lld\n", (long long)mass_after_rand);
    fprintf(fp, "  , chargeBefore      := %lld\n", (long long)charge_before);
    fprintf(fp, "  , chargeAfter       := %lld\n", (long long)charge_after);
    fprintf(fp, "  , wireErrors        := %d\n",   r14.wire_ownership_errors);
    fprintf(fp, "  , russianDollDepth  := %d\n",   r14.russian_doll_max_depth);
    fprintf(fp, "  , tunDryPairs       := %d\n",   r14.tun_active_pairs_dry);
    fprintf(fp, "  , tunDryPower       := %lld\n", (long long)r14.tun_power_burn_dry);
    fprintf(fp, "  , tunRehydRewrites  := %d\n",   r14.tun_rewrites_rehyd);
    fprintf(fp, "  , ssinArbiterPairs  := %d\n",   r14.ssin_arbiter_pairings);
    fprintf(fp, "  , ssinContended     := %d\n",   r14.ssin_agents_contended);
    fprintf(fp, "  , ssinConfluentRw   := %d\n",   r14.ssin_confluent_rewrites);
    fprintf(fp, "  , causalHops        := %d\n",   r14.causal_4layer_hops);
    fprintf(fp, "  , threeGenMassDrift := %lld\n", (long long)r15.three_genome_mass_drift);
    fprintf(fp, "  , syn3aTotalGenes   := %d\n",   r16.syn3a_total_genes);
    fprintf(fp, "  , syn3aDarkGenes    := %d\n",   r16.syn3a_dark_genes);
    fprintf(fp, "  , syn3aDarkDiscov   := %d\n",   r16.syn3a_dark_discovered);
    fprintf(fp, "  , sixTierIntDrift   := %lld\n", (long long)r16.six_tier_int_drift);
    fprintf(fp, "  , pmPassed          := %d\n",   sc.pm_passed);
    fprintf(fp, "  , mfPassed          := %d\n",   sc.mf_passed);
    fprintf(fp, "  , gwPassed          := %d\n",   sc.gw_passed);
    fprintf(fp, "  , cubeOk            := %d\n",   sc.cube_ok);
    fprintf(fp, "  , cubeAdapt         := %d\n",   sc.cube_adapt);
    fprintf(fp, "  , cubeOver          := %d\n",   sc.cube_over);
    fprintf(fp, "  , cubeExcl          := %d\n",   sc.cube_excl);
    fprintf(fp, "  , cubeActiveVerif   := %d\n",   sc.cube_active_verified);
    fprintf(fp, "  , cubeVerifiedTot   := %d\n",   sc.cube_verified_total);
    fprintf(fp, "  , quantumCapQ       := %d\n",   sc.quantum_cap_q);
    fprintf(fp, "  , quantumCapBytes   := %d\n",   sc.quantum_cap_bytes);
    fprintf(fp, "  , b158TritStates    := %d\n",   sc.b158_trit_states);
    fprintf(fp, "  , shaFifo           := \"%s\"\n", r14.sha_fifo);
    fprintf(fp, "  , shaLifo           := \"%s\"\n", r14.sha_lifo);
    fprintf(fp, "  , shaRandom         := \"%s\"\n", r14.sha_random);
    fprintf(fp, "  , tb14Sha256        := \"%s\"\n", r14.sha256);
    fprintf(fp, "  , tb16Sha256        := \"%s\"\n", r16.sha256);
    fprintf(fp, "  , cubeSha256        := \"%s\"\n", sc.sha256);
    fprintf(fp, "  , receiptSha256     := \"%s\"\n", receipt_sha);
    fprintf(fp, "  }\n\n");
    fprintf(fp, "end\n\n");
    fprintf(fp, "end OmegaCellular.LiveC11Receipt\n");
    fclose(fp);

    printf("Exported Live C11 Lean 4 Trace Receipt -> %s (Seed=%llu, Rewrites=%d/%d/%d, Mass=%lld, Charge=%lld, 6TierDrift=%lld, Substrates=%d/75, Cube=%d/%d/%d/%d, SHA=%s, CubeSHA=%s)\n",
           out_path, (unsigned long long)seed, r14.rewrites_fifo, r14.rewrites_lifo, r14.rewrites_random,
           (long long)mass_before, (long long)charge_before, (long long)r16.six_tier_int_drift,
           sc.substrates_passed, sc.cube_ok, sc.cube_adapt, sc.cube_over, sc.cube_excl,
           receipt_sha, sc.sha256);
    return 0;
}

static const char* cube_badge_str(uint8_t badge) {
    if (badge == OMEGA_CUBE_OK)    return "[OK]";
    if (badge == OMEGA_CUBE_ADAPT) return "[ADAPT]";
    if (badge == OMEGA_CUBE_OVER)  return "[OVER]";
    return "[EXCL]";
}

static int run_cube_cli(uint64_t seed, const char* json_path) {
    OmegaSubstratesCubeResult sc;
    omega_run_substrates_75_and_cube_1000(seed, &sc);

    printf("================================================================================\n");
    printf(" CELLULAR ENGINE OMEGA v%s — 75 SUBSTRATES & 1,000-CELL HYBRID CUBE (Seed %llu)\n",
           OMEGA_VERSION_STR, (unsigned long long)seed);
    printf("================================================================================\n");
    printf("[PM01..PM25] Numeric Precision Codecs  : %2d / %2d PASSED | MaxRMSE=%.5f | PM18 Kahan=%.2e (<1e-12) | b1.58=3^5=%d<=256\n",
           sc.pm_passed, sc.pm_total, sc.pm_max_rmse, sc.pm18_kahan_rel_err, sc.b158_trit_states);
    printf("[MF01..MF25] Model Family Kernels      : %2d / %2d PASSED | MaxNorm=%.4f (<1e4)\n",
           sc.mf_passed, sc.mf_total, sc.mf_max_norm);
    printf("[GW01..GW25] Geometry/Topology Builders: %2d / %2d PASSED | MaxInvErr=%.2e\n",
           sc.gw_passed, sc.gw_total, sc.gw_max_invariant_err);
    printf("[SUBSTRATES] Clean-Slate v3.0 Total    : %2d / %2d PASSED\n",
           sc.substrates_passed, sc.substrates_total);
    printf("[HYBRID-10³] 10x10x10 Partition        : %d [OK] | %d [ADAPT] | %d [OVER] | %d [EXCL] | Total=%d\n",
           sc.cube_ok, sc.cube_adapt, sc.cube_over, sc.cube_excl, sc.cube_total);
    printf("[RUNTIME]    Active Triples Verified   : %d / %d (OK+ADAPT w/ S2S) | OVER=%d | EXCL=%d | Total=%d/1000 | MaxNorm=%.4f\n",
           sc.cube_active_verified, sc.cube_ok + sc.cube_adapt,
           sc.cube_over_flagged, sc.cube_excl_flagged, sc.cube_verified_total, sc.cube_max_norm);
    printf("[QUANTUM]    Coherent Qubit Cap        : Q<=%d -> 16*2^%d = %d B (16 KiB)\n",
           sc.quantum_cap_q, sc.quantum_cap_q, sc.quantum_cap_bytes);
    printf("--------------------------------------------------------------------------------\n");
    printf(" COMPLETE in %.2f ms | CUBE SHA-256: %s\n", sc.elapsed_ms, sc.sha256);
    printf("================================================================================\n");

    if (json_path && json_path[0] != '\0') {
        FILE* fp = fopen(json_path, "wb");
        if (!fp) {
            fprintf(stderr, "ERROR: Failed to open %s for writing cube JSON.\n", json_path);
            return 1;
        }
        fprintf(fp, "{\n");
        fprintf(fp, "  \"schema\": \"cellular-engine-omega-substrates-cube/v%s\",\n", OMEGA_VERSION_STR);
        fprintf(fp, "  \"version\": \"%s\",\n", OMEGA_VERSION_STR);
        fprintf(fp, "  \"seed\": %llu,\n", (unsigned long long)seed);
        fprintf(fp, "  \"elapsed_ms\": %.3f,\n", sc.elapsed_ms);
        fprintf(fp, "  \"sha256\": \"%s\",\n", sc.sha256);
        fprintf(fp, "  \"quantum_cap_q\": %d,\n", sc.quantum_cap_q);
        fprintf(fp, "  \"quantum_cap_bytes\": %d,\n", sc.quantum_cap_bytes);
        fprintf(fp, "  \"b158_trit_states\": %d,\n", sc.b158_trit_states);
        fprintf(fp, "  \"substrates_75\": {\n");
        fprintf(fp, "    \"pm_passed\": %d, \"pm_total\": %d, \"pm_max_rmse\": %.8f, \"pm18_kahan_rel_err\": %.15e,\n",
                sc.pm_passed, sc.pm_total, sc.pm_max_rmse, sc.pm18_kahan_rel_err);
        fprintf(fp, "    \"mf_passed\": %d, \"mf_total\": %d, \"mf_max_norm\": %.8f,\n",
                sc.mf_passed, sc.mf_total, sc.mf_max_norm);
        fprintf(fp, "    \"gw_passed\": %d, \"gw_total\": %d, \"gw_max_invariant_err\": %.15e,\n",
                sc.gw_passed, sc.gw_total, sc.gw_max_invariant_err);
        fprintf(fp, "    \"total_passed\": %d, \"total\": %d,\n", sc.substrates_passed, sc.substrates_total);
        fprintf(fp, "    \"pm\": [\n");
        for (int i = 0; i < 25; ++i) {
            fprintf(fp, "      {\"id\": \"%s\", \"name\": \"%s\", \"bytes_per_syn\": %.6f, \"rmse\": %.10f, \"passed\": %s}%s\n",
                    sc.pm[i].id, sc.pm[i].name, sc.pm[i].bytes_per_syn, sc.pm[i].rmse,
                    sc.pm[i].passed ? "true" : "false", (i + 1 < 25) ? "," : "");
        }
        fprintf(fp, "    ],\n");
        fprintf(fp, "    \"mf\": [\n");
        for (int i = 0; i < 25; ++i) {
            fprintf(fp, "      {\"id\": \"%s\", \"name\": \"%s\", \"state_norm\": %.8f, \"energy_proxy\": %.8f, \"passed\": %s}%s\n",
                    sc.mf[i].id, sc.mf[i].name, sc.mf[i].state_norm, sc.mf[i].energy_proxy,
                    sc.mf[i].passed ? "true" : "false", (i + 1 < 25) ? "," : "");
        }
        fprintf(fp, "    ],\n");
        fprintf(fp, "    \"gw\": [\n");
        for (int i = 0; i < 25; ++i) {
            fprintf(fp, "      {\"id\": \"%s\", \"name\": \"%s\", \"invariant_val\": %.8f, \"invariant_err\": %.12e, \"passed\": %s}%s\n",
                    sc.gw[i].id, sc.gw[i].name, sc.gw[i].invariant_val, sc.gw[i].invariant_err,
                    sc.gw[i].passed ? "true" : "false", (i + 1 < 25) ? "," : "");
        }
        fprintf(fp, "    ]\n");
        fprintf(fp, "  },\n");
        fprintf(fp, "  \"hybrid_cube_1000\": {\n");
        fprintf(fp, "    \"total\": %d,\n", sc.cube_total);
        fprintf(fp, "    \"ok\": %d,\n", sc.cube_ok);
        fprintf(fp, "    \"adapt\": %d,\n", sc.cube_adapt);
        fprintf(fp, "    \"over\": %d,\n", sc.cube_over);
        fprintf(fp, "    \"excl\": %d,\n", sc.cube_excl);
        fprintf(fp, "    \"active_verified\": %d,\n", sc.cube_active_verified);
        fprintf(fp, "    \"over_flagged\": %d,\n", sc.cube_over_flagged);
        fprintf(fp, "    \"excl_flagged\": %d,\n", sc.cube_excl_flagged);
        fprintf(fp, "    \"verified_total\": %d,\n", sc.cube_verified_total);
        fprintf(fp, "    \"max_norm\": %.8f,\n", sc.cube_max_norm);
        fprintf(fp, "    \"cells_1000\": [\n");
        for (int z = 0; z < 10; ++z) {
            for (int y = 0; y < 10; ++y) {
                for (int x = 0; x < 10; ++x) {
                    int idx = z * 100 + y * 10 + x;
                    fprintf(fp, "      {\"id\": \"X%dY%dZ%d\", \"x\": %d, \"y\": %d, \"z\": %d, \"badge\": \"%s\", \"norm\": %.5f, \"verified\": %s}%s\n",
                            x, y, z, x, y, z, cube_badge_str(sc.cube_badge[idx]),
                            (double)sc.cube_norm[idx], sc.cube_pass[idx] ? "true" : "false",
                            (idx + 1 < 1000) ? "," : "");
                }
            }
        }
        fprintf(fp, "    ]\n");
        fprintf(fp, "  }\n");
        fprintf(fp, "}\n");
        fclose(fp);
        printf("Wrote 75-Substrate + 1,000-Cube JSON receipt -> %s\n", json_path);
    }

    int all_ok = (sc.pm_passed == 25 && sc.mf_passed == 25 && sc.gw_passed == 25 &&
                  sc.substrates_passed == 75 &&
                  sc.cube_ok == 661 && sc.cube_adapt == 274 && sc.cube_over == 20 && sc.cube_excl == 45 &&
                  sc.cube_active_verified == 935 && sc.cube_over_flagged == 20 &&
                  sc.cube_excl_flagged == 45 && sc.cube_verified_total == 1000 &&
                  sc.pm18_kahan_rel_err < 1e-12 && sc.mf_max_norm < 1e4 && sc.cube_max_norm < 1e4);
    if (!all_ok) {
        fprintf(stderr, "FAIL: 75-Substrate / 1,000-Cube invariant verification failed!\n");
        return 1;
    }
    return 0;
}

static void compute_master_hash(
    const TB01Result* r1,  const TB02Result* r2,  const TB03Result* r3,  const TB04Result* r4,
    const TB05Result* r5,  const TB06Result* r6,  const TB07Result* r7,  const TB08Result* r8,
    const TB09Result* r9,  const TB10Result* r10, const TB11Result* r11, const TB12Result* r12,
    const TB13Result* r13, const TB14Result* r14, const TB15Result* r15, const TB16Result* r16,
    char master_hash[65]) {
    char concat[16 * 64 + 1];
    memcpy(concat +  0*64, r1->sha256, 64);  memcpy(concat +  1*64, r2->sha256, 64);
    memcpy(concat +  2*64, r3->sha256, 64);  memcpy(concat +  3*64, r4->sha256, 64);
    memcpy(concat +  4*64, r5->sha256, 64);  memcpy(concat +  5*64, r6->sha256, 64);
    memcpy(concat +  6*64, r7->sha256, 64);  memcpy(concat +  7*64, r8->sha256, 64);
    memcpy(concat +  8*64, r9->sha256, 64);  memcpy(concat +  9*64, r10->sha256, 64);
    memcpy(concat + 10*64, r11->sha256, 64); memcpy(concat + 11*64, r12->sha256, 64);
    memcpy(concat + 12*64, r13->sha256, 64); memcpy(concat + 13*64, r14->sha256, 64);
    memcpy(concat + 14*64, r15->sha256, 64); memcpy(concat + 15*64, r16->sha256, 64);
    concat[16*64] = '\0';
    omega_sha256_hex(concat, 16 * 64, master_hash);
}

static void emit_json_all(uint64_t seed, int mutant, int feral) {
    uint8_t dna[256]; make_demo_autophage_dna(seed, dna);
    TB01Result r1; TB02Result r2; TB03Result r3; TB04Result r4;
    TB05Result r5; TB06Result r6; TB07Result r7; TB08Result r8;
    TB09Result r9; TB10Result r10; TB11Result r11; TB12Result r12;
    TB13Result r13; TB14Result r14; TB15Result r15; TB16Result r16;
    OmegaTierLedger ledger;

    double t0 = omega_time_sec();
    uint64_t tb15_seed = feral ? cli_feral_mix(seed) : seed;
    omega_run_tb01(seed, mutant, &r1);
    omega_run_tb02(seed, mutant, &r2);
    omega_run_tb03(seed, mutant, &r3);
    omega_run_tb04(seed, mutant, &r4);
    omega_run_tb05(seed, mutant, &r5);
    omega_run_tb06(seed, mutant, &r6);
    omega_run_tb07(seed, mutant, &r7);
    omega_run_tb08(seed, mutant, &r8);
    omega_run_tb09(seed, mutant, &r9);
    omega_run_tb10(seed, mutant, &r10);
    omega_run_tb11(seed, mutant, &r11);
    omega_run_tb12(seed, mutant, dna, &r12);
    omega_run_tb13(seed, mutant, feral, &r13);
    omega_run_tb14(seed, mutant, feral, &r14);
    omega_run_tb15(tb15_seed, mutant, &r15);
    build_tier_ledger(&r4, &r5, &r13, &r14, &r15, &ledger);
    omega_run_tb16_coupled(seed, mutant, &ledger, &r16);
    double elapsed_ms = (omega_time_sec() - t0) * 1000.0;

    char master_hash[65];
    compute_master_hash(&r1, &r2, &r3, &r4, &r5, &r6, &r7, &r8,
                        &r9, &r10, &r11, &r12, &r13, &r14, &r15, &r16, master_hash);

    printf("{\n");
    printf("  \"version\": \"%s\", \"seed\": %llu, \"mutant\": %d, \"feral\": %d, \"elapsed_ms\": %.3f, \"master_sha256\": \"%s\",\n",
           OMEGA_VERSION_STR, (unsigned long long)seed, mutant, feral, elapsed_ms, master_hash);
    printf("  \"tb01\": {\"scallop_net_disp\": %.12f, \"scallop_peak\": %.6f, \"purcell_2dof_disp\": %.6f, \"loop_area\": %.4f, \"mota5_disp\": %.4f, \"mota5_steps\": %d, \"mutant_disp\": %.6f, \"sha256\": \"%s\"},\n",
           r1.scallop_net_disp, r1.scallop_peak_excursion, r1.purcell_2dof_disp, r1.purcell_loop_area, r1.mota5_helical_disp, r1.mota5_proton_steps, r1.mutant_disp, r1.sha256);
    printf("  \"tb02\": {\"shannon_no_phage\": %.4f, \"shannon_with_phage\": %.4f, \"top_no_phage\": %.4f, \"top_with_phage\": %.4f, \"phikz_no_shell\": %.4f, \"phikz_shell\": %.4f, \"amoeba_no_mavirus\": %.4f, \"amoeba_with_mavirus\": %.4f, \"orfans\": %d, \"obelisks\": %d, \"sha256\": \"%s\"},\n",
           r2.shannon_no_phage, r2.shannon_with_phage, r2.top_strain_no_phage, r2.top_strain_with_phage,
           r2.phikz_survival_no_shell, r2.phikz_survival_shell, r2.amoeba_no_mavirus, r2.amoeba_with_mavirus,
           r2.orfan_genes_invented, r2.obelisk_colonized_cells, r2.sha256);
    printf("  \"tb03\": {\"centrosome_init\": %.4f, \"centrosome_final\": %.4f, \"centrosome_xy\": [%.3f,%.3f], \"mt_before_ca\": %.4f, \"mt_after_ca\": %.4f, \"mt_captured\": %d, \"kin_dom\": %.4f, \"dyn_dom\": %.4f, \"parked_ca\": %.5f, \"arp23_pN\": %.4f, \"lacry_ratio\": %.2f, \"lacry_strain\": %.5f, \"ray_before\": ",
           r3.centrosome_init_err, r3.centrosome_final_err, (double)r3.centrosome_xy[0], (double)r3.centrosome_xy[1],
           r3.mt_mean_len_before_ca, r3.mt_mean_len_after_ca, r3.mt_captured_count,
           r3.cargo_net_kinesin_dom, r3.cargo_net_dynein_dom, r3.cargo_parked_high_ca,
           r3.arp23_70deg_thrust_pN, r3.lacrymaria_max_ratio, r3.lacrymaria_mem_strain);
    print_float_array(r3.ray_len_before_ca, 32);
    printf(", \"ray_after\": ");
    print_float_array(r3.ray_len_after_ca, 32);
    printf(", \"ray_captured\": ");
    print_u8_array(r3.ray_captured, 32);
    printf(", \"sha256\": \"%s\"},\n", r3.sha256);
    printf("  \"tb04\": {\"eq_droplets\": %d, \"eq_radius\": %.4f, \"active_droplets\": %d, \"active_radius\": %.4f, \"mass_drift\": %.12f, \"mass_drift_active\": %.12f, \"eq_field\": ",
           r4.eq_droplet_count, r4.eq_mean_radius, r4.active_droplet_count, r4.active_mean_radius, r4.mass_drift_eq, r4.mass_drift_active);
    print_float_array(r4.eq_field, 1024);
    printf(", \"active_field\": ");
    print_float_array(r4.active_field, 1024);
    printf(", \"sha256\": \"%s\"},\n", r4.sha256);
    printf("  \"tb05\": {\"n_vertices\": %d, \"init_charge\": %.10f, \"final_charge\": %.10f, \"init_half_defects\": %d, \"final_organizers\": %d, \"max_err\": %.12f, \"nematic_S\": %.4f, \"defects\": ",
           r5.n_vertices, r5.init_charge_sum, r5.final_charge_sum, r5.init_half_defects, r5.final_polar_organizers, r5.max_charge_error, r5.nematic_order_S);
    print_double_array(r5.defect_coords, 12);
    printf(", \"sha256\": \"%s\"},\n", r5.sha256);
    printf("  \"tb06\": {\"eq_L100\": %.5f, \"eq_L600\": %.5f, \"kp1_L600\": %.5f, \"kp2_L3200\": %.5f, \"gtp_diss\": %.1f, \"L\": ",
           r6.master_eq_L100, r6.master_eq_L600, r6.master_kp1_L600, r6.master_kp2_L3200, r6.gtp_dissipation_kp2);
    print_double_array(r6.curve_L, 7);
    printf(", \"eq\": "); print_double_array(r6.curve_eq, 7);
    printf(", \"kp1\": "); print_double_array(r6.curve_kp1, 7);
    printf(", \"kp2\": "); print_double_array(r6.curve_kp2, 7);
    printf(", \"sha256\": \"%s\"},\n", r6.sha256);
    printf("  \"tb07\": {\"eq_diff_std\": %.6f, \"fast_inh_std\": %.6f, \"feinberg_d0_amp\": %.8f, \"feinberg_d1_amp\": %.6f, \"turing_field\": ",
           r7.turing_eq_diff_std, r7.turing_fast_inh_std, r7.feinberg_d0_osc_amp, r7.feinberg_d1_osc_amp);
    print_float_array(r7.turing_field, 1024);
    printf(", \"sha256\": \"%s\"},\n", r7.sha256);
    printf("  \"tb08\": {\"tur_min\": %.4f, \"tur_jitter_ratio\": %.4f, \"drives\": ", r8.tur_product_min, r8.tur_jitter_ratio);
    print_double_array(r8.tur_drives, 5);
    printf(", \"products\": "); print_double_array(r8.tur_products, 5);
    printf(", \"adapt_err\": %.6f, \"drift_gain\": %.4f, \"stentor_tier\": %d, \"stentor_escaped\": %d, \"sha256\": \"%s\"},\n",
           r8.chemotaxis_adapt_err, r8.chemotaxis_drift_gain, r8.stentor_max_tier, r8.stentor_escaped, r8.sha256);
    printf("  \"tb09\": {\"total_cells\": %d, \"organs\": [%d,%d,%d,%d,%d,%d,%d], \"gut_sealed\": %d, \"hox\": %d, \"axons_wired\": %d, \"axons_pruned\": %d, \"cpg_walk\": %.4f, \"food_gain\": %.4f, \"bioglass_chi\": %.4f, \"intact_surv\": %.2f, \"ko_surv\": %.2f, \"mega_neurons\": %d, \"mega_nuclei\": %d, \"sha256\": \"%s\"},\n",
           r9.total_cells, r9.organ_counts[0], r9.organ_counts[1], r9.organ_counts[2], r9.organ_counts[3],
           r9.organ_counts[4], r9.organ_counts[5], r9.organ_counts[6], r9.gut_lumen_sealed, r9.hox_genes_active,
           r9.axons_wired, r9.axons_pruned, r9.cpg_walk_distance, r9.cpg_food_bias_gain, r9.cahs_bioglass_chi,
           r9.intact_survival_pct, r9.ko_survival_pct, r9.megaphragma_neurons, r9.megaphragma_nuclei, r9.sha256);
    printf("  \"tb10\": {\"passive_work\": %.8f, \"odd_work\": %.6f, \"theory_odd_work\": %.6f, \"recip_vel\": %.6f, \"nonrecip_vel\": %.6f, \"psi6\": %.4f, \"sha256\": \"%s\"},\n",
           r10.passive_cycle_work, r10.odd_cycle_work, r10.theoretical_odd_work, r10.reciprocal_chase_vel,
           r10.nonrecip_chase_vel, r10.crystal_hex_order, r10.sha256);
    printf("  \"tb11\": {\"prok_bigNe\": %.2f, \"prok_smallNe\": %.2f, \"euk_bigNe\": %.2f, \"euk_smallNe\": %.2f, \"gen_bigNe\": %.1f, \"gen_smallNe\": %.1f, \"trans_atp_share\": %.4f, \"nitro_sync\": %.4f, \"nitro_n2\": %d, \"sha256\": \"%s\"},\n",
           r11.max_vol_prok_largeNe, r11.max_vol_prok_smallNe, r11.max_vol_euk_largeNe, r11.max_vol_euk_smallNe,
           r11.genome_kb_euk_largeNe, r11.genome_kb_euk_smallNe, r11.translation_atp_share,
           r11.nitroplast_sync_ratio, r11.nitroplast_n2_fixed, r11.sha256);
    printf("  \"tb12\": {\"trackA_tumors\": %d, \"trackB_tumors\": %d, \"ovi_feral\": %.2f, \"ovi_50pct\": %.2f, \"ovi_100pct\": %.2f, \"escapes_50pct\": %d, \"asmo_mean\": %.4f, \"asmo_std\": %.4f, \"asmo_112\": ",
           r12.trackA_tumors, r12.trackB_tumors, r12.ovi_feral_baseline, r12.ovi_restraint_50pct,
           r12.ovi_restraint_100pct, r12.escape_mutations_50pct, r12.asmodensius_mean, r12.asmodensius_std);
    print_float_array(r12.asmodensius_112, 112);
    printf(", \"sha256\": \"%s\"},\n", r12.sha256);
    printf("  \"tb13\": {\"epochs\": %d, \"lineages\": %d, \"niches\": %d, \"nov1\": %d, \"nov2\": %d, \"chimeras\": %d, \"swarms\": %d, \"creatures\": %d, \"outliers\": %d, \"vol\": %.4f, \"world_shift\": %.3f, \"flux\": %.1f, \"mass_res\": %.12f, \"apex\": \"%s\", \"axis_means\": ",
           r13.epochs_run, r13.active_lineages, r13.unique_niches_found, r13.novelty_first_half,
           r13.novelty_second_half, r13.russian_doll_chimeras, r13.swarm_colonies_formed,
           r13.organ_creatures_born, r13.wild_outliers_spawned, r13.morphospace_volume,
           r13.world_niche_shift, r13.repo_5layer_flux, r13.global_mass_residual, r13.apex_archetype);
    print_double_array(r13.axis_means, 12);
    printf(", \"axis_spans\": ");
    print_double_array(r13.axis_spans, 12);
    printf(", \"sha256\": \"%s\"},\n", r13.sha256);
    printf("  \"tb14\": {\"nodes_init\": %d, \"rewrites_fifo\": %d, \"rewrites_lifo\": %d, \"rewrites_random\": %d, \"layer_rewrites\": [%d,%d,%d,%d], \"confluence_match\": %d, \"init_mass\": %lld, \"init_charge\": %lld, \"mass_drift_fifo\": %lld, \"mass_drift_lifo\": %lld, \"mass_drift_random\": %lld, \"gap_charge_drift\": %lld, \"wire_errors\": %d, \"max_depth\": %d, \"tun_dry_pairs\": %d, \"tun_dry_power\": %lld, \"tun_rehyd_rewrites\": %d, \"ssin_arbiter\": %d, \"ssin_contended\": %d, \"ssin_confluent\": %d, \"causal_hops\": %d, \"ssin_confluent_pct\": %.2f, \"rewrites_per_sec\": %.1f, \"sha_fifo\": \"%s\", \"sha_lifo\": \"%s\", \"sha_random\": \"%s\", \"sha256\": \"%s\"},\n",
           r14.total_nodes_init, r14.rewrites_fifo, r14.rewrites_lifo, r14.rewrites_random,
           r14.layer_rewrites[0], r14.layer_rewrites[1], r14.layer_rewrites[2], r14.layer_rewrites[3],
           r14.confluence_exact_match, (long long)r14.init_mass_q, (long long)r14.init_charge_q,
           (long long)r14.mass_drift_fifo, (long long)r14.mass_drift_lifo, (long long)r14.mass_drift_random,
           (long long)r14.gap_charge_drift, r14.wire_ownership_errors, r14.russian_doll_max_depth,
           r14.tun_active_pairs_dry, (long long)r14.tun_power_burn_dry, r14.tun_rewrites_rehyd,
           r14.ssin_arbiter_pairings, r14.ssin_agents_contended, r14.ssin_confluent_rewrites,
           r14.causal_4layer_hops, r14.ssin_confluent_pct,
           r14.rewrites_per_sec, r14.sha_fifo, r14.sha_lifo, r14.sha_random, r14.sha256);
    printf("  \"tb15\": {\"ciss_racemic_ee\": %.6f, \"ciss_magnetite_ee\": %.6f, \"raf_rxns\": %d, \"raf_mass_res\": %.12f, \"oxy_mds\": %d, \"oxy_ies_pct\": %.2f, \"tad_gain\": %.3f, \"tad_insul\": %.3f, \"cohesin_blocks\": %d, \"aa_codons\": %d, \"trp_qy_gain\": %.3f, \"trp_subrad\": %.2f, \"ocelloid_gain\": %.3f, \"cnidocyte_g\": %.1f, \"magnetosome_cos\": %.4f, \"three_gen_drift\": %lld, \"three_gen_ko_blocked\": %d, \"klepto_gain\": %.3f, \"levin_wild_heads\": %d, \"levin_reprog_heads\": %d, \"levin_dna_mut\": %d, \"somite_cycles\": %d, \"emt_switches\": %d, \"wired_reach\": %.4f, \"wireless_reach\": %.4f, \"rhombozoa_cells\": %d, \"henneguya_mt\": %d, \"vmem_wild\": ",
           r15.ciss_racemic_ee, r15.ciss_magnetite_ee, r15.raf_closed_reactions, r15.raf_mass_residual,
           r15.oxytricha_mds_unscrambled, r15.oxytricha_ies_deleted_pct, r15.tad_contact_gain, r15.tad_ctcf_insulation,
           r15.cohesin_steric_blocks, r15.expanded_aa_codons, r15.trp_superradiant_qy_gain, r15.trp_subradiant_ratio,
           r15.ocelloid_focal_gain, r15.cnidocyte_peak_accel_g, r15.magnetosome_align_cos,
           (long long)r15.three_genome_mass_drift, r15.three_gen_ko_blocked, r15.kleptoplasty_surv_gain,
           r15.levin_wild_heads, r15.levin_reprog_heads, r15.levin_dna_mutations,
           r15.somite_clock_cycles, r15.emt_reversible_switches,
           r15.connectome_wired_reach, r15.connectome_wireless_reach,
           r15.rhombozoa_cells, r15.henneguya_mt_genes);
    print_float_array(r15.levin_vmem_wild, 32);
    printf(", \"vmem_2head\": ");
    print_float_array(r15.levin_vmem_2head, 32);
    printf(", \"cnid_curve\": ");
    print_float_array(r15.cnidocyte_accel_curve, 32);
    printf(", \"tad_profile\": ");
    print_float_array(r15.tad_contact_profile, 32);
    printf(", \"sha256\": \"%s\"},\n", r15.sha256);
    printf("  \"tb16\": {\"manifold_dim\": %d, \"zones\": %d, \"zonation_mono\": %.5f, \"raw_r2\": %.5f, \"perturb_r2\": %.5f, \"essential_auroc\": %.5f, \"syn3a_total\": %d, \"syn3a_dark\": %d, \"syn3a_discovered\": %d, \"raw_ai_halluc\": %.6f, \"proj_mass_res\": %.12f, \"six_tier_err\": %.12f, \"six_tier_int_drift\": %lld, \"z_o2\": ",
           r16.manifold_dim, r16.zonation_zones, r16.zonation_monotonicity, r16.zeroshot_raw_r2, r16.zeroshot_perturb_r2,
           r16.zeroshot_essential_auroc, r16.syn3a_total_genes, r16.syn3a_dark_genes, r16.syn3a_dark_discovered,
           r16.raw_ai_mass_hallucination, r16.projected_mass_residual, r16.six_tier_closure_err, (long long)r16.six_tier_int_drift);
    print_float_array(r16.zonation_o2, 32);
    printf(", \"z_gluconeo\": ");
    print_float_array(r16.zonation_gluconeo, 32);
    printf(", \"z_glycolysis\": ");
    print_float_array(r16.zonation_glycolysis, 32);
    printf(", \"ko_hist\": ");
    print_float_array(r16.syn3a_ko_flux_hist, 32);
    printf(", \"sha256\": \"%s\"}\n}\n", r16.sha256);
}

static void run_human_all(uint64_t seed, int mutant, int feral) {
    uint8_t dna[256]; make_demo_autophage_dna(seed, dna);
    TB01Result r1; TB02Result r2; TB03Result r3; TB04Result r4;
    TB05Result r5; TB06Result r6; TB07Result r7; TB08Result r8;
    TB09Result r9; TB10Result r10; TB11Result r11; TB12Result r12;
    TB13Result r13; TB14Result r14; TB15Result r15; TB16Result r16;
    OmegaTierLedger ledger;

    double t0 = omega_time_sec();
    uint64_t tb15_seed = feral ? cli_feral_mix(seed) : seed;
    omega_run_tb01(seed, mutant, &r1);
    omega_run_tb02(seed, mutant, &r2);
    omega_run_tb03(seed, mutant, &r3);
    omega_run_tb04(seed, mutant, &r4);
    omega_run_tb05(seed, mutant, &r5);
    omega_run_tb06(seed, mutant, &r6);
    omega_run_tb07(seed, mutant, &r7);
    omega_run_tb08(seed, mutant, &r8);
    omega_run_tb09(seed, mutant, &r9);
    omega_run_tb10(seed, mutant, &r10);
    omega_run_tb11(seed, mutant, &r11);
    omega_run_tb12(seed, mutant, dna, &r12);
    omega_run_tb13(seed, mutant, feral, &r13);
    omega_run_tb14(seed, mutant, feral, &r14);
    omega_run_tb15(tb15_seed, mutant, &r15);
    build_tier_ledger(&r4, &r5, &r13, &r14, &r15, &ledger);
    omega_run_tb16_coupled(seed, mutant, &ledger, &r16);
    double dt_ms = (omega_time_sec() - t0) * 1000.0;

    printf("================================================================================\n");
    if (mutant > 0) {
        printf(" CELLULAR ENGINE OMEGA v%s — 16-TESTBED LEDGER (Seed %llu, MUTANT M%d%s)\n",
               OMEGA_VERSION_STR, (unsigned long long)seed, mutant, feral ? ", FERAL-HW" : "");
    } else {
        printf(" CELLULAR ENGINE OMEGA v%s — 16-TESTBED DEEP-STRATA & VIRTUAL-CELL LEDGER (Seed %llu%s)\n",
               OMEGA_VERSION_STR, (unsigned long long)seed, feral ? ", FERAL-HW" : "");
    }
    printf("================================================================================\n");
    printf("[TB01] Purcell Scallop & MotA5  | Scallop=%.2e (Peak=%.3f) | 2-DOF=%.4f (Area=%.2f) | MotA5=%.2f um (%d H+ steps)\n",
           r1.scallop_net_disp, r1.scallop_peak_excursion, r1.purcell_2dof_disp, r1.purcell_loop_area, r1.mota5_helical_disp, r1.mota5_proton_steps);
    printf("[TB02] Virosphere Ecosystem     | Shannon: %.3f->%.3f | PhiKZ Shell: %.3f->%.3f | Mavirus Rescue: %.2f->%.2f | ORFans=%d Obelisks=%d\n",
           r2.shannon_no_phage, r2.shannon_with_phage, r2.phikz_survival_no_shell, r2.phikz_survival_shell,
           r2.amoeba_no_mavirus, r2.amoeba_with_mavirus, r2.orfan_genes_invented, r2.obelisk_colonized_cells);
    printf("[TB03] Cytoskeleton & Motors    | CenterErr: %.2f->%.2f um | Ca2+ Melt: %.2f->%.2f um | Kin=+%.2f Dyn=%.2f | Arp2/3=%.1fpN | Lacry=%.1fx\n",
           r3.centrosome_init_err, r3.centrosome_final_err, r3.mt_mean_len_before_ca, r3.mt_mean_len_after_ca,
           r3.cargo_net_kinesin_dom, r3.cargo_net_dynein_dom, r3.arp23_70deg_thrust_pN, r3.lacrymaria_max_ratio);
    printf("[TB04] Active-Arrested LLPS     | Eq Ostwald: %d drops (R=%.2f, drift=%.1e) | Active ATP: %d drops (R=%.2f)\n",
           r4.eq_droplet_count, r4.eq_mean_radius, r4.mass_drift_eq, r4.active_droplet_count, r4.active_mean_radius);
    printf("[TB05] Poincare-Hopf Sphere     | V=%d | Sum(q): %.8f -> %.8f (maxErr=%.2e) | Defects=%d | S=%.3f\n",
           r5.n_vertices, r5.init_charge_sum, r5.final_charge_sum, r5.max_charge_error, r5.final_polar_organizers, r5.nematic_order_S);
    printf("[TB06] Eigen & Proofreading     | Eq(L=100)=%.3f Eq(L=600)=%.5f | KP1(L=600)=%.3f | KP2(L=3200)=%.4f\n",
           r6.master_eq_L100, r6.master_eq_L600, r6.master_kp1_L600, r6.master_kp2_L3200);
    printf("[TB07] Turing & Feinberg CRNT   | EqDiffStd=%.5f -> FastInhStd=%.4f | Feinberg d0=%.6f vs d1=%.3f\n",
           r7.turing_eq_diff_std, r7.turing_fast_inh_std, r7.feinberg_d0_osc_amp, r7.feinberg_d1_osc_amp);
    printf("[TB08] TUR, Bayes & Stentor     | TUR_min=%.3f (>=2.0) | JitterRatio=%.2fx | AdaptErr=%.5f | Stentor Tier=%d (Esc=%d)\n",
           r8.tur_product_min, r8.tur_jitter_ratio, r8.chemotaxis_adapt_err, r8.stentor_max_tier, r8.stentor_escaped);
    printf("[TB09] <=1000-Cell Tardigrade   | Cells=%d (4-Hox, GutSealed=%d) | Walk=%.2f | Bioglass=%.3f | Surv: %.1f%% vs %.2f%% KO | Mega=%d->%d\n",
           r9.total_cells, r9.gut_lumen_sealed, r9.cpg_walk_distance, r9.cahs_bioglass_chi,
           r9.intact_survival_pct, r9.ko_survival_pct, r9.megaphragma_neurons, r9.megaphragma_nuclei);
    printf("[TB10] Odd Chiral & Chase       | PassiveW=%.2e vs OddW=%.4f (Theory=%.4f) | Recip=%.4f vs Chase=%.4f | psi6=%.3f\n",
           r10.passive_cycle_work, r10.odd_cycle_work, r10.theoretical_odd_work,
           r10.reciprocal_chase_vel, r10.nonrecip_chase_vel, r10.crystal_hex_order);
    printf("[TB11] Lane vs Lynch + Nitro    | ProkV=%.1f vs EukV=%.1f | Gen(Ne400)=%.0f vs Gen(Ne40)=%.0f kb | TransATP=%.1f%% | NitroSync=%.2f\n",
           r11.max_vol_prok_largeNe, r11.max_vol_euk_largeNe, r11.genome_kb_euk_largeNe,
           r11.genome_kb_euk_smallNe, 100.0 * r11.translation_atp_share, r11.nitroplast_sync_ratio);
    printf("[TB12] Track A/B + 4-Build      | Tumors: A=%d vs B=%d | OVI: %.1f / %.1f(50%%,%d esc) / %.1f(100%%) | Asmo112: mu=%.3f sd=%.3f\n",
           r12.trackA_tumors, r12.trackB_tumors, r12.ovi_feral_baseline, r12.ovi_restraint_50pct,
           r12.escape_mutations_50pct, r12.ovi_restraint_100pct, r12.asmodensius_mean, r12.asmodensius_std);
    printf("[TB13] 5-Repo Open-Prolif 12D   | Niches=%d (H1=%d H2=%d) | Chimeras=%d Swarms=%d Creat=%d Outliers=%d | Res=%.1e | Apex=%s\n",
           r13.unique_niches_found, r13.novelty_first_half, r13.novelty_second_half, r13.russian_doll_chimeras,
           r13.swarm_colonies_formed, r13.organ_creatures_born, r13.wild_outliers_spawned,
           r13.global_mass_residual, r13.apex_archetype);
    printf("[TB14] Bio-Frostbite INet VM    | Rewrites=%d/%d/%d (L1=%d L2=%d L3=%d L4=%d, Causal=%dh) | Confluent=%s | Drift=%lld | QErr=%lld | Depth=%d | Tun0W=%d/%lld | SSIN=%.1f%% (%da->%dp)\n",
           r14.rewrites_fifo, r14.rewrites_lifo, r14.rewrites_random,
           r14.layer_rewrites[0], r14.layer_rewrites[1], r14.layer_rewrites[2], r14.layer_rewrites[3],
           r14.causal_4layer_hops,
           r14.confluence_exact_match ? "EXACT-5xSHA256" : "MISMATCH!",
           (long long)r14.mass_drift_fifo, (long long)r14.gap_charge_drift,
           r14.russian_doll_max_depth, r14.tun_active_pairs_dry, (long long)r14.tun_power_burn_dry,
           r14.ssin_confluent_pct, r14.ssin_agents_contended, r14.ssin_arbiter_pairings);
    printf("[TB15] Deep Strata Frontier     | CISS=%.1f%% | TAD=%.1fx (Steric=%d) | TrpSR=%.2fx | Cnid=%.2fMg | Levin=%d->%dH (%d-DNA) | Wire=%.1f%%->%.1f%% | 3GenDrift=%lld (KO=%d/3)\n",
           100.0 * r15.ciss_magnetite_ee, r15.tad_contact_gain, r15.cohesin_steric_blocks, r15.trp_superradiant_qy_gain,
           r15.cnidocyte_peak_accel_g / 1.0e6, r15.levin_wild_heads, r15.levin_reprog_heads, r15.levin_dna_mutations,
           100.0 * r15.connectome_wired_reach, 100.0 * r15.connectome_wireless_reach,
           (long long)r15.three_genome_mass_drift, r15.three_gen_ko_blocked);
    printf("[TB16] Virtual-Cell AI & syn3A  | Zonation r=%.3f | ZeroShot R2=%.3f (Raw=%.3f) AUROC=%.3f | Dark=%d/%d | RawAI=%.3f -> ProjRes=%.1e | 6Tier=%.1e\n",
           r16.zonation_monotonicity, r16.zeroshot_perturb_r2, r16.zeroshot_raw_r2, r16.zeroshot_essential_auroc,
           r16.syn3a_dark_discovered, r16.syn3a_dark_genes,
           r16.raw_ai_mass_hallucination, r16.projected_mass_residual, r16.six_tier_closure_err);

    char master_hash[65];
    compute_master_hash(&r1, &r2, &r3, &r4, &r5, &r6, &r7, &r8,
                        &r9, &r10, &r11, &r12, &r13, &r14, &r15, &r16, master_hash);
    printf("--------------------------------------------------------------------------------\n");
    printf(" ALL 16 TESTBEDS COMPLETE in %.2f ms | MASTER SHA-256: %s\n", dt_ms, master_hash);
    printf("================================================================================\n");
}

static int parse_u64_strict(const char* s, uint64_t* out) {
    if (!s || s[0] == '\0' || s[0] == '-' || s[0] == '+') return 0;
    uint64_t acc = 0ULL;
    for (size_t i = 0; s[i] != '\0'; ++i) {
        if (s[i] < '0' || s[i] > '9') return 0;
        uint64_t d = (uint64_t)(s[i] - '0');
        if (acc > (UINT64_MAX - d) / 10ULL) return 0;
        acc = acc * 10ULL + d;
    }
    if (acc == 0ULL) return 0;
    *out = acc;
    return 1;
}

static int parse_mutant_strict(const char* s, int* out) {
    if (!s || s[0] == '\0') return 0;
    int acc = 0;
    for (size_t i = 0; s[i] != '\0'; ++i) {
        if (s[i] < '0' || s[i] > '9') return 0;
        acc = acc * 10 + (int)(s[i] - '0');
        if (acc > 16) return 0;
    }
    *out = acc;
    return 1;
}

static void print_cli_usage(FILE* fp) {
    fprintf(fp,
            "CELLULAR ENGINE OMEGA v%s\n"
            "Usage: omega_cell [all | json | cube [out.json]] [--seed N] [--mutant 0..16] [--feral]\n"
            "       omega_cell --export-lean-trace <LiveC11Receipt.lean> [--seed N]\n"
            "       omega_cell --version | --help\n",
            OMEGA_VERSION_STR);
}

int main(int argc, char** argv) {
    uint64_t seed = 2026001ULL;
    int mutant = 0, feral = 0;
    const char* cmd = "all";
    const char* export_path = NULL;
    const char* cube_json_path = NULL;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-v") == 0) {
            printf("cellular-engine-omega %s\n", OMEGA_VERSION_STR);
            return 0;
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_cli_usage(stdout);
            return 0;
        } else if (strcmp(argv[i], "--seed") == 0) {
            if (i + 1 >= argc || !parse_u64_strict(argv[i + 1], &seed)) {
                fprintf(stderr, "ERROR: --seed requires a positive 64-bit integer (>= 1).\n");
                return 2;
            }
            ++i;
        } else if (strcmp(argv[i], "--mutant") == 0) {
            if (i + 1 >= argc || !parse_mutant_strict(argv[i + 1], &mutant)) {
                fprintf(stderr, "ERROR: --mutant requires an integer in [0, 16].\n");
                return 2;
            }
            ++i;
        } else if (strcmp(argv[i], "--feral") == 0) {
            feral = 1;
        } else if (strcmp(argv[i], "--export-lean-trace") == 0) {
            if (i + 1 >= argc || argv[i + 1][0] == '\0' || argv[i + 1][0] == '-') {
                fprintf(stderr, "ERROR: --export-lean-trace requires an output file path.\n");
                return 2;
            }
            export_path = argv[++i];
        } else if (strcmp(argv[i], "cube") == 0) {
            cmd = "cube";
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                cube_json_path = argv[++i];
            }
        } else if (strcmp(argv[i], "json") == 0 || strcmp(argv[i], "all") == 0) {
            cmd = argv[i];
        } else {
            fprintf(stderr, "ERROR: Unknown argument '%s'.\n", argv[i]);
            print_cli_usage(stderr);
            return 2;
        }
    }

    if (export_path) {
        return export_lean_trace(export_path, seed);
    }
    if (strcmp(cmd, "cube") == 0) {
        return run_cube_cli(seed, cube_json_path);
    }
    if (strcmp(cmd, "json") == 0) {
        emit_json_all(seed, mutant, feral);
    } else {
        run_human_all(seed, mutant, feral);
    }
    return 0;
}
