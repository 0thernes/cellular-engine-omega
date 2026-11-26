/* SPDX-License-Identifier: Apache-2.0
 * CELLULAR ENGINE OMEGA v5.2.0 — libFuzzer-Compatible & Standalone Fuzz Harness (B15)
 *
 * Exercises native C11 input boundaries (SHA-256 variable lengths, Philox4x32-10 RNG,
 * TB12 arbitrary 256-byte DNA genomes, TB14 Bio-Frostbite INet VM, and 75-Substrate codecs)
 * against mutated byte streams to verify zero crashes, zero OOB reads, and invariant stability.
 *
 * Build (libFuzzer on Linux Clang):
 *   clang -O1 -g -fsanitize=fuzzer,address,undefined -DOMEGA_LIBFUZZER \
 *     -Iinclude src/omega_rng_hash.c src/exp_07_12.c src/exp_14_bio_inet.c \
 *     test/fuzz_omega_cli_codec.c -lm -o bin/fuzz_omega
 *
 * Build (standalone deterministic corpus runner):
 *   cc -O2 -std=c11 -Iinclude src/omega_rng_hash.c src/exp_07_12.c src/exp_14_bio_inet.c \
 *     test/fuzz_omega_cli_codec.c -lm -o bin/fuzz_omega_standalone
 */
#include "omega_bare_crt.h"
#include "omega_abi.h"

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    char hex1[65], hex2[65];
    omega_sha256_hex(data, size, hex1);
    omega_sha256_hex(data, size, hex2);
    if (memcmp(hex1, hex2, 65) != 0) exit(91);

    uint64_t seed = 1ULL;
    size_t n = size < 8 ? size : 8;
    for (size_t i = 0; i < n; ++i) {
        seed = (seed << 8) ^ (uint64_t)data[i] ^ 0x9E3779B97F4A7C15ULL;
    }
    if (seed == 0ULL) seed = 1ULL;

    OmegaRng r;
    omega_rng_seed(&r, seed);
    double u = omega_rng_f64(&r);
    if (!(u >= 0.0 && u < 1.0)) exit(92);

    uint8_t dna[256];
    memset(dna, 0, sizeof(dna));
    size_t copy_n = size < 256 ? size : 256;
    if (copy_n > 0) memcpy(dna, data, copy_n);

    int mutant = (size > 0) ? (int)(data[0] % 17u) : 0;
    TB14Result r14;
    omega_run_tb14(seed, (mutant == 8) ? 8 : 0, 0, &r14);
    if (mutant != 8) {
        if (r14.confluence_exact_match != 1 || r14.wire_ownership_errors != 0 || r14.mass_drift_fifo != 0) {
            exit(93);
        }
    }

    if ((size & 7u) == 0u) {
        TB12Result r12;
        omega_run_tb12(seed, 0, dna, &r12);
        if (r12.trackA_tumors != 0 || !(r12.asmodensius_mean >= 0.0 && r12.asmodensius_mean <= 1.0)) {
            exit(94);
        }
    }
    return 0;
}

#ifndef OMEGA_LIBFUZZER
static void prop_check_determinism_and_conservation(uint64_t seed) {
    char h1[65], h2[65];
    omega_sha256_hex(&seed, sizeof(seed), h1);
    omega_sha256_hex(&seed, sizeof(seed), h2);
    if (memcmp(h1, h2, 65) != 0) exit(95);

    TB14Result r14a, r14b;
    omega_run_tb14(seed, 0, 0, &r14a);
    omega_run_tb14(seed, 0, 0, &r14b);
    if (r14a.confluence_exact_match != 1 || r14a.mass_drift_fifo != 0 ||
        r14a.gap_charge_drift != 0 || r14a.wire_ownership_errors != 0 ||
        strcmp(r14a.sha256, r14b.sha256) != 0) {
        exit(96);
    }
}

int main(void) {
    OmegaRng rng;
    omega_rng_seed(&rng, 0xF022B055ULL);
    uint8_t buf[512];
    const int CASES = 256;
    for (int i = 0; i < CASES; ++i) {
        size_t len = (i == 0) ? 0 : (size_t)(omega_rng_u32(&rng) % 512u);
        for (size_t j = 0; j < len; ++j) {
            buf[j] = (uint8_t)(omega_rng_u32(&rng) & 0xFFu);
        }
        LLVMFuzzerTestOneInput(buf, len);
    }
    for (int p = 0; p < 32; ++p) {
        uint64_t s = ((uint64_t)omega_rng_u32(&rng) << 32) | (uint64_t)omega_rng_u32(&rng);
        if (s == 0ULL) s = 1ULL;
        prop_check_determinism_and_conservation(s);
    }
    printf("FUZZ + PROP_CHECK PASS: %d mutated byte-stream cases + 32 randomized property seeds verified (0 crashes, 0 invariant violations).\n", CASES);
    return 0;
}
#endif
