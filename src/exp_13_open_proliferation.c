#include "omega_bare_crt.h"
#include "omega_abi.h"

/*
 * TB13 — THE 5-REPO INTERCONNECTED 12-AXIS CONTINUOUS MORPHOSPACE & FERAL OPEN-PROLIFERATION ENGINE
 * Connects:
 *   REPO 1: DNA / Genetic OS (AUTOPHAGE mutation, Oxytricha unscrambling, ADAR RNA edit, Codon dialects)
 *   REPO 2: CELL + VIROSPHERE (12-Axis continuous vector z[0..11], HGT, LLPS, exotic engines)
 *   REPO 3: CREATURES / ORGANS / TISSUES (Asmodensius eutelic organs, EMT, zonation, Ocelloids)
 *   REPO 4: SWARMS / COLONIES / IDENTITIES (Russian-doll depth 1..3 chimeras, Cable wires, Rosettes)
 *   REPO 5: ENVIRONMENT / DOME / WORLD (5 dynamic biomes with reciprocal niche construction & chaos!)
 */

#define N_LINEAGES 96
#define N_DOMES    5
#define HASH_SLOTS 65536

typedef struct MorphoLineage {
    double z[12];       /* Continuous 12-axis cellular morphospace coordinate in [0, 1]^12 */
    double biomass;     /* Conserved mass quantum in this lineage */
    double fitness;
    int    dome;        /* Current environmental dome (0..4) */
    int    nest_depth;  /* Russian-doll endosymbiotic nesting depth (0..3) */
    int    is_swarm;    /* Part of active colony / cable-wire / rosette / slug */
    int    is_creature; /* Compiled multicellular organ-tissue creature */
} MorphoLineage;

typedef struct BiomeDome {
    double target[12];  /* Dynamic environmental attractor profile across 12 axes */
    double free_mass;   /* Conserved abiotic mass pool in this dome */
    double stress;      /* Radiation / Desiccation / Redox shock intensity */
} BiomeDome;

static inline double clamp01(double v) {
    if (v < 0.0) return 0.0;
    if (v > 1.0) return 1.0;
    return v;
}

static uint32_t quantize_12axis_hash(const double z[12]) {
    /* Map continuous [0,1]^12 into 4^12 = 16,777,216 distinct morphospace hypercells */
    uint32_t h = 2166136261u;
    for (int a = 0; a < 12; ++a) {
        uint32_t bin = (uint32_t)(z[a] * 3.9999);
        h ^= bin + (uint32_t)(a * 17);
        h *= 16777619u;
    }
    return h;
}

static uint64_t hardware_feral_mix(uint64_t base_seed) {
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

OMEGA_API void omega_run_tb13(uint64_t seed, int mutant_mode, int feral_hw_entropy, TB13Result* out) {
    memset(out, 0, sizeof(*out));
    uint64_t eff_seed = feral_hw_entropy ? hardware_feral_mix(seed) : seed;
    OmegaRng rng; omega_rng_seed(&rng, eff_seed ^ 0x13131313ULL);

    /* Mutant M7 freezes feral open proliferation & HGT to verify falsifiability */
    int prolif_on = (mutant_mode == 7) ? 0 : 1;

    /* Initialize 5 Wild Environmental Domes (Repo 5: Environment/Dome/World):
     * 0: Abyssal Hydrothermal Vent (GDGT monolayers, electron bifurcation, Asgard tentacles)
     * 1: Sunlit Pelagic Ocean (Diatoms, Nitroplasts, Ocelloids, Giant Caulerpa, Virosphere)
     * 2: Hyper-Saline Brine / Soil (Haloquadratum squares, Cable bacteria, Fungal lassos)
     * 3: Deep Granite Radiolysis Crust (Ultra-slow division, CPR/DPANN, Chernobyl radiotrophs)
     * 4: Desiccating Moss & UV Radiation Dome (<=1000-cell Tardigrade CAHS/Dsup/DODA1/TRID1) */
    BiomeDome domes[N_DOMES];
    const double init_targets[N_DOMES][12] = {
        { 0.25, 0.70, 0.30, 0.45, 0.35, 0.78, 0.55, 0.40, 0.20, 0.35, 0.65, 0.30 },
        { 0.75, 0.55, 0.65, 0.60, 0.85, 0.55, 0.82, 0.88, 0.35, 0.70, 0.75, 0.60 },
        { 0.45, 0.80, 0.50, 0.50, 0.40, 0.88, 0.70, 0.65, 0.60, 0.55, 0.60, 0.75 },
        { 0.12, 0.35, 0.20, 0.75, 0.15, 0.95, 0.65, 0.30, 0.80, 0.40, 0.85, 0.25 },
        { 0.60, 0.85, 0.55, 0.65, 0.60, 0.45, 0.60, 0.75, 0.95, 0.82, 0.70, 0.90 }
    };
    double init_total_mass = 0.0;
    for (int d = 0; d < N_DOMES; ++d) {
        for (int a = 0; a < 12; ++a) domes[d].target[a] = init_targets[d][a];
        domes[d].free_mass = 1200.0;
        domes[d].stress    = 0.15 + 0.12 * (double)d;
        init_total_mass   += domes[d].free_mass;
    }

    MorphoLineage* pop = (MorphoLineage*)calloc(N_LINEAGES, sizeof(MorphoLineage));
    uint32_t* seen_keys = (uint32_t*)calloc(HASH_SLOTS, sizeof(uint32_t));
    uint8_t*  seen_used = (uint8_t*)calloc(HASH_SLOTS, sizeof(uint8_t));
    if (!pop || !seen_keys || !seen_used) {
        free(pop); free(seen_keys); free(seen_used);
        return;
    }
    for (int i = 0; i < N_LINEAGES; ++i) {
        pop[i].dome = i % N_DOMES;
        pop[i].biomass = 20.0;
        pop[i].fitness = 1.0;
        pop[i].nest_depth = 0;
        pop[i].is_swarm = 0;
        pop[i].is_creature = 0;
        for (int a = 0; a < 12; ++a) {
            /* Start clustered tightly near ancestral LUCA protocell center (0.25) */
            pop[i].z[a] = clamp01(0.25 + 0.04 * omega_rng_gauss(&rng));
        }
        init_total_mass += pop[i].biomass;
    }

    int unique_niches = 0, nov_first = 0, nov_second = 0;
    int russian_dolls = 0, swarms = 0, creatures = 0, outliers = 0;
    double world_shift_accum = 0.0, cross_flux_accum = 0.0;
    const int EPOCHS = 240;

    for (int ep = 0; ep < EPOCHS; ++ep) {
        /* 1. Chaos & Environmental Shock Waves (Repo 5 Dynamic Gradients!) */
        for (int d = 0; d < N_DOMES; ++d) {
            double shift = prolif_on ? (0.018 * sin(0.09 * ep + 1.3 * d) + 0.012 * omega_rng_gauss(&rng)) : 0.0;
            int ax = (ep + d * 3) % 12;
            double old_v = domes[d].target[ax];
            domes[d].target[ax] = clamp01(old_v + shift);
            world_shift_accum += fabs(domes[d].target[ax] - old_v);
        }

        /* 2. Evaluate Fitness, Conserved Mass Metabolism, HGT, Russian-Doll Fusion & Proliferation */
        for (int i = 0; i < N_LINEAGES; ++i) {
            MorphoLineage* c = &pop[i];
            BiomeDome* dm = &domes[c->dome];

            /* Distance to dynamic environmental optimum + exotic metabolic bonuses */
            double d2 = 0.0;
            for (int a = 0; a < 12; ++a) {
                double diff = c->z[a] - dm->target[a];
                d2 += diff * diff;
            }
            /* Flavin electron bifurcation / Cable redox / ADAR RNA editing / Cryptobiosis adaptation */
            double exotic_bonus = 0.25 * c->z[5] + 0.20 * c->z[3] + 0.25 * c->z[8] * dm->stress;
            c->fitness = exp(-1.2 * d2) + exotic_bonus;

            /* Exact Conserved Mass Transfer between World Dome <-> Cell Lineage (O40!) */
            double uptake = (dm->free_mass > 1.0) ? (0.45 * c->fitness) : 0.0;
            if (uptake > dm->free_mass) uptake = dm->free_mass;
            double decay = 0.38 * (1.0 - 0.35 * c->z[8]) * (c->biomass * 0.05);
            if (decay > c->biomass - 1.0) decay = (c->biomass > 1.0) ? (c->biomass - 1.0) : 0.0;
            dm->free_mass -= uptake;
            c->biomass    += uptake;
            c->biomass    -= decay;
            dm->free_mass += decay;
            cross_flux_accum += uptake + decay;

            /* Reciprocal Niche Construction: thriving cells physically reshape their dome! */
            if (prolif_on && c->biomass > 22.0) {
                int ax_nc = (i + ep) % 12;
                double delta_nc = 0.004 * (c->z[ax_nc] - dm->target[ax_nc]);
                dm->target[ax_nc] = clamp01(dm->target[ax_nc] - delta_nc); /* depletion/modification */
                world_shift_accum += fabs(delta_nc);
            }

            /* Feral Open Proliferation: Mutation + Virosphere HGT + ADAR RNA Edit + Migration */
            if (prolif_on) {
                /* Adaptive gradient climb + chaotic erratic jumps (feral improvisation!) */
                int feral_jump = (omega_rng_f64(&rng) < 0.09);
                double step_sd = feral_jump ? 0.22 : 0.045;
                for (int a = 0; a < 12; ++a) {
                    double grad = 0.055 * (dm->target[a] - c->z[a]);
                    double noise = step_sd * omega_rng_gauss(&rng);
                    c->z[a] = clamp01(c->z[a] + grad + noise);
                }
                /* Horizontal Gene Transfer (Repo 1 <-> Repo 2 <-> Repo 4) via Virosphere / T4SS */
                if (omega_rng_f64(&rng) < 0.14) {
                    int donor = (int)(omega_rng_u32(&rng) % N_LINEAGES);
                    int gene_ax = (int)(omega_rng_u32(&rng) % 12u);
                    c->z[gene_ax] = 0.5 * (c->z[gene_ax] + pop[donor].z[gene_ax]);
                    cross_flux_accum += 0.5;
                }
                /* Russian-Doll Endosymbiosis Fusion (Depth 0 -> 1 -> 2 -> 3!) */
                if (c->z[4] > 0.55 && c->nest_depth < 3 && omega_rng_f64(&rng) < 0.05) {
                    c->nest_depth++;
                    if (c->nest_depth >= 2) russian_dolls++;
                }
                /* Swarm / Colony / Cable-Wire / Rosette Formation (Repo 4) */
                if (c->z[11] > 0.62 && !c->is_swarm) {
                    c->is_swarm = 1;
                    swarms++;
                }
                /* Multicellular Organ-Tissue Creature Emergence (Repo 3: Asmodensius!) */
                if (c->z[11] > 0.72 && c->z[9] > 0.55 && c->z[0] > 0.45 && !c->is_creature) {
                    c->is_creature = 1;
                    creatures++;
                }
                /* Extreme Wild Outlier Detection (margins <0.08 or >0.92 across exotic axes!) */
                if (c->z[0] < 0.08 || c->z[0] > 0.92 || c->z[2] < 0.06 || c->z[2] > 0.93 ||
                    c->z[5] > 0.92 || c->z[7] > 0.92 || c->z[8] > 0.92) {
                    if (ep % 6 == 0) outliers++;
                }
                /* Cross-Dome Migration */
                if (omega_rng_f64(&rng) < 0.03) {
                    c->dome = (c->dome + 1 + (int)(omega_rng_u32(&rng) % (N_DOMES - 1))) % N_DOMES;
                }
            }

            /* Record Novel 12-Axis Morphospace Hypercell Discovery (Open Proliferation Metric!) */
            uint32_t key = quantize_12axis_hash(c->z);
            uint32_t slot = key & (HASH_SLOTS - 1);
            for (int probe = 0; probe < 64; ++probe) {
                uint32_t idx = (slot + (uint32_t)probe) & (HASH_SLOTS - 1);
                if (!seen_used[idx]) {
                    seen_used[idx] = 1;
                    seen_keys[idx] = key;
                    unique_niches++;
                    if (ep < EPOCHS / 2) nov_first++;
                    else nov_second++;
                    break;
                }
                if (seen_keys[idx] == key) break;
            }
        }
    }

    /* Exact Global 5-Repo Conservation Verification & 12-Axis Morphospace Span */
    double final_total_mass = 0.0;
    for (int d = 0; d < N_DOMES; ++d) final_total_mass += domes[d].free_mass;
    int alive = 0;
    int apex_i = 0;
    double best_fit = -1.0;

    double mins[12], maxs[12], sums[12];
    for (int a = 0; a < 12; ++a) { mins[a] = 1e9; maxs[a] = -1e9; sums[a] = 0.0; }

    for (int i = 0; i < N_LINEAGES; ++i) {
        final_total_mass += pop[i].biomass;
        if (pop[i].biomass > 0.5) alive++;
        if (pop[i].fitness > best_fit) { best_fit = pop[i].fitness; apex_i = i; }
        for (int a = 0; a < 12; ++a) {
            double v = pop[i].z[a];
            sums[a] += v;
            if (v < mins[a]) mins[a] = v;
            if (v > maxs[a]) maxs[a] = v;
        }
    }

    double vol_sum = 0.0;
    for (int a = 0; a < 12; ++a) {
        out->axis_means[a] = sums[a] / (double)N_LINEAGES;
        out->axis_spans[a] = maxs[a] - mins[a];
        vol_sum += out->axis_spans[a];
    }

    out->epochs_run            = EPOCHS;
    out->active_lineages       = alive;
    out->unique_niches_found   = unique_niches;
    out->novelty_first_half    = nov_first;
    out->novelty_second_half   = nov_second;
    out->russian_doll_chimeras = russian_dolls;
    out->swarm_colonies_formed = swarms;
    out->organ_creatures_born  = creatures;
    out->wild_outliers_spawned = outliers;
    out->morphospace_volume    = vol_sum / 12.0; /* Mean fractional span across all 12 axes */
    out->world_niche_shift     = world_shift_accum;
    out->repo_5layer_flux      = cross_flux_accum;
    out->global_mass_residual  = fabs(final_total_mass - init_total_mass);

    /* Name emergent wild apex phenotype purely from its 12-axis coordinates */
    const double* az = pop[apex_i].z;
    const char* s_meta = (az[5] > 0.72) ? "Radiotroph/Cable" : (az[5] > 0.45 ? "Chemo-Phototroph" : "Anaerobic");
    const char* s_body = (az[11] > 0.68) ? "Eutelic-Creature" : (az[2] > 0.68 ? "Giant-Coenocyte" : (az[0] < 0.25 ? "Nano-Episymbiont" : "Protist-Chimera"));
    const char* s_ext  = (az[8] > 0.68) ? "Bioglass-Tun" : (az[7] > 0.68 ? "Ocelloid/Harpoon" : "ADAR-Plastic");
    snprintf(out->apex_archetype, sizeof(out->apex_archetype), "%s %s (%s, D%d)", s_meta, s_body, s_ext, pop[apex_i].nest_depth);

    free(pop);
    free(seen_keys);
    free(seen_used);

    omega_sha256_hex(out, offsetof(TB13Result, sha256), out->sha256);
}
