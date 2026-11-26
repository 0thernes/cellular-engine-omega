#include "omega_bare_crt.h"
#include "omega_abi.h"

/* ============================================================================
 * Philox4x32-10 Counter-Based Deterministic PRNG (Salmon et al. SC11)
 * ============================================================================ */
static void philox4x32_10_block(uint32_t ctr0, uint32_t ctr1, uint32_t key0, uint32_t key1, uint32_t out[4]) {
    uint32_t c0 = ctr0, c1 = ctr1, c2 = 0u, c3 = 0u;
    uint32_t k0 = key0, k1 = key1;
    const uint32_t M0 = 0xD2511F53u;
    const uint32_t M1 = 0xCD9E8D57u;
    const uint32_t W0 = 0x9E3779B9u;
    const uint32_t W1 = 0xBB67AE85u;
    for (int round = 0; round < 10; ++round) {
        uint64_t p0 = (uint64_t)M0 * (uint64_t)c0;
        uint64_t p1 = (uint64_t)M1 * (uint64_t)c2;
        uint32_t hi0 = (uint32_t)(p0 >> 32), lo0 = (uint32_t)p0;
        uint32_t hi1 = (uint32_t)(p1 >> 32), lo1 = (uint32_t)p1;
        uint32_t nc0 = hi1 ^ c1 ^ k0;
        uint32_t nc1 = lo1;
        uint32_t nc2 = hi0 ^ c3 ^ k1;
        uint32_t nc3 = lo0;
        c0 = nc0; c1 = nc1; c2 = nc2; c3 = nc3;
        k0 += W0;
        k1 += W1;
    }
    out[0] = c0; out[1] = c1; out[2] = c2; out[3] = c3;
}

OMEGA_API void omega_rng_seed(OmegaRng* r, uint64_t seed) {
    r->key = seed ^ 0x9E3779B97F4A7C15ULL;
    r->ctr = 0ULL;
    r->idx = 4;
}

OMEGA_API uint32_t omega_rng_u32(OmegaRng* r) {
    if (r->idx >= 4) {
        philox4x32_10_block((uint32_t)r->ctr, (uint32_t)(r->ctr >> 32),
                            (uint32_t)r->key, (uint32_t)(r->key >> 32), r->buf);
        r->ctr++;
        r->idx = 0;
    }
    return r->buf[r->idx++];
}

OMEGA_API double omega_rng_f64(OmegaRng* r) {
    uint64_t hi = (uint64_t)(omega_rng_u32(r) >> 5);
    uint64_t lo = (uint64_t)(omega_rng_u32(r) >> 6);
    return (double)((hi << 26) | lo) * (1.0 / 9007199254740992.0);
}

OMEGA_API double omega_rng_gauss(OmegaRng* r) {
    double u1 = omega_rng_f64(r);
    double u2 = omega_rng_f64(r);
    if (u1 < 1e-12) u1 = 1e-12;
    return sqrt(-2.0 * log(u1)) * cos(6.2831853071795864769 * u2);
}

/* ============================================================================
 * Monotonic Wall-Clock Timer
 * ============================================================================ */
OMEGA_API double omega_time_sec(void) {
#if defined(_WIN32)
#  if defined(OMEGA_BARE_WIN64)
    int64_t freq = 1, cnt = 0;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&cnt);
    return (double)cnt / (double)freq;
#  else
    LARGE_INTEGER freq, cnt;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&cnt);
    return (double)cnt.QuadPart / (double)freq.QuadPart;
#  endif
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
#endif
}

/* ============================================================================
 * Canonical FIPS 180-4 SHA-256 (Observer-Independent P9 Hashing)
 * ============================================================================ */
static const uint32_t SHA_K[64] = {
    0x428a2f98u,0x71374491u,0xb5c0fbcfu,0xe9b5dba5u,0x3956c25bu,0x59f111f1u,0x923f82a4u,0xab1c5ed5u,
    0xd807aa98u,0x12835b01u,0x243185beu,0x550c7dc3u,0x72be5d74u,0x80deb1feu,0x9bdc06a7u,0xc19bf174u,
    0xe49b69c1u,0xefbe4786u,0x0fc19dc6u,0x240ca1ccu,0x2de92c6fu,0x4a7484aau,0x5cb0a9dcu,0x76f988dau,
    0x983e5152u,0xa831c66du,0xb00327c8u,0xbf597fc7u,0xc6e00bf3u,0xd5a79147u,0x06ca6351u,0x14292967u,
    0x27b70a85u,0x2e1b2138u,0x4d2c6dfcu,0x53380d13u,0x650a7354u,0x766a0abbu,0x81c2c92eu,0x92722c85u,
    0xa2bfe8a1u,0xa81a664bu,0xc24b8b70u,0xc76c51a3u,0xd192e819u,0xd6990624u,0xf40e3585u,0x106aa070u,
    0x19a4c116u,0x1e376c08u,0x2748774cu,0x34b0bcb5u,0x391c0cb3u,0x4ed8aa4au,0x5b9cca4fu,0x682e6ff3u,
    0x748f82eeu,0x78a5636fu,0x84c87814u,0x8cc70208u,0x90befffau,0xa4506cebu,0xbef9a3f7u,0xc67178f2u
};

static inline uint32_t ror32(uint32_t x, uint32_t n) { return (x >> n) | (x << (32u - n)); }

static void sha256_transform(uint32_t st[8], const uint8_t blk[64]) {
    uint32_t w[64];
    for (int i = 0; i < 16; ++i) {
        w[i] = ((uint32_t)blk[4*i] << 24) | ((uint32_t)blk[4*i+1] << 16) |
               ((uint32_t)blk[4*i+2] << 8) | (uint32_t)blk[4*i+3];
    }
    for (int i = 16; i < 64; ++i) {
        uint32_t s0 = ror32(w[i-15], 7) ^ ror32(w[i-15], 18) ^ (w[i-15] >> 3);
        uint32_t s1 = ror32(w[i-2], 17) ^ ror32(w[i-2], 19) ^ (w[i-2] >> 10);
        w[i] = w[i-16] + s0 + w[i-7] + s1;
    }
    uint32_t a=st[0], b=st[1], c=st[2], d=st[3], e=st[4], f=st[5], g=st[6], h=st[7];
    for (int i = 0; i < 64; ++i) {
        uint32_t S1 = ror32(e, 6) ^ ror32(e, 11) ^ ror32(e, 25);
        uint32_t ch = (e & f) ^ ((~e) & g);
        uint32_t t1 = h + S1 + ch + SHA_K[i] + w[i];
        uint32_t S0 = ror32(a, 2) ^ ror32(a, 13) ^ ror32(a, 22);
        uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
        uint32_t t2 = S0 + maj;
        h = g; g = f; f = e; e = d + t1;
        d = c; c = b; b = a; a = t1 + t2;
    }
    st[0]+=a; st[1]+=b; st[2]+=c; st[3]+=d; st[4]+=e; st[5]+=f; st[6]+=g; st[7]+=h;
}

OMEGA_API void omega_sha256_hex(const void* data, size_t len, char out_hex[65]) {
    uint32_t st[8] = {
        0x6a09e667u,0xbb67ae85u,0x3c6ef372u,0xa54ff53au,
        0x510e527fu,0x9b05688cu,0x1f83d9abu,0x5be0cd19u
    };
    const uint8_t* p = (const uint8_t*)data;
    size_t off = 0;
    while (off + 64 <= len) {
        sha256_transform(st, p + off);
        off += 64;
    }
    uint8_t tail[128];
    memset(tail, 0, sizeof(tail));
    size_t rem = len - off;
    if (rem > 0) memcpy(tail, p + off, rem);
    tail[rem] = 0x80u;
    size_t pad_len = (rem < 56) ? 64 : 128;
    uint64_t bitlen = (uint64_t)len * 8ULL;
    for (int i = 0; i < 8; ++i) tail[pad_len - 1 - i] = (uint8_t)(bitlen >> (8 * i));
    sha256_transform(st, tail);
    if (pad_len == 128) sha256_transform(st, tail + 64);

    static const char HEX[] = "0123456789abcdef";
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            out_hex[i*8 + j] = HEX[(st[i] >> (28 - 4*j)) & 0xFu];
        }
    }
    out_hex[64] = '\0';
}
