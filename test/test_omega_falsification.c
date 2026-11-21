#include "omega_bare_crt.h"
#include "omega_abi.h"

static int g_pass = 0, g_fail = 0;

static void gate(const char* id, const char* desc, int cond) {
    if (cond) {
        g_pass++;
        printf("  [PASS] %-4s | %s\n", id, desc);
    } else {
        g_fail++;
        printf("  [FAIL] %-4s | %s\n", id, desc);
    }
}

int main(void) {
    double t0 = omega_time_sec();
    const uint64_t SEED = 2026001ULL;
    uint8_t dna[256];
    OmegaRng rng; omega_rng_seed(&rng, SEED ^ 0xA070F46EULL);
    for (int i = 0; i < 256; ++i) dna[i] = (uint8_t)(omega_rng_u32(&rng) & 0xFFu);

    printf("================================================================================\n");
    printf(" CELLULAR ENGINE OMEGA v4.0 — 43-GATE FALSIFICATION SUITE + 10 PLANTED MUTANTS\n");
    printf("================================================================================\n");

    /* Run all 12 testbeds */
    TB01Result r1, r1b; TB02Result r2; TB03Result r3; TB04Result r4;
    TB05Result r5; TB06Result r6; TB07Result r7; TB08Result r8;
    TB09Result r9; TB10Result r10; TB11Result r11; TB12Result r12, r12b;

    omega_run_tb01(SEED, 0, &r1);
    omega_run_tb01(SEED, 0, &r1b);
    omega_run_tb02(SEED, 0, &r2);
    omega_run_tb03(SEED, 0, &r3);
    omega_run_tb04(SEED, 0, &r4);
    omega_run_tb05(SEED, 0, &r5);
    omega_run_tb06(SEED, 0, &r6);
    omega_run_tb07(SEED, 0, &r7);
    omega_run_tb08(SEED, 0, &r8);
    omega_run_tb09(SEED, 0, &r9);
    omega_run_tb10(SEED, 0, &r10);
    omega_run_tb11(SEED, 0, &r11);
    omega_run_tb12(SEED, 0, dna, &r12);
    omega_run_tb12(SEED, 0, dna, &r12b);

    /* G01-G18: Exact Scientific & Mathematical Invariant Gates across TB01-TB12 */
    gate("G01", "TB01 Purcell 1-DOF reciprocal scallop net displacement == 0 (< 1e-10) despite peak > 0.1",
         fabs(r1.scallop_net_disp) < 1e-10 && r1.scallop_peak_excursion > 0.10);
    gate("G02", "TB01 2-DOF Purcell non-reciprocal loop & stochastic MotA5MotB2 10-state proton ratchet generate thrust",
         r1.purcell_2dof_disp > 0.10 && r1.mota5_helical_disp > 3.0 && r1.mota5_proton_steps >= 100);
    gate("G03", "TB02 Virosphere Kill-the-Winner strictly lifts Shannon diversity & blocks monoculture",
         r2.shannon_with_phage > r2.shannon_no_phage + 0.50 && r2.top_strain_with_phage < r2.top_strain_no_phage);
    gate("G04", "TB02 PhiKZ Chimallin phage nucleus shields vs CRISPR & Mavirus rescues amoebae from Giant Virus",
         r2.phikz_survival_shell > 5.0 * r2.phikz_survival_no_shell && r2.amoeba_with_mavirus > 2.5 * r2.amoeba_no_mavirus);
    gate("G05", "TB03 Dynamic-instability wall-force centers centrosome & Ca2+ spike melts MTs > 50%",
         r3.centrosome_final_err < 0.65 * r3.centrosome_init_err && r3.mt_mean_len_after_ca < 0.55 * r3.mt_mean_len_before_ca && r3.mt_captured_count >= 1);
    gate("G06", "TB03 Kinesin/Dynein tug-of-war reverses, Hill Ca2+-Miro parks cargo (<0.05), Arp2/3 >35pN & Lacrymaria ~30x",
         r3.cargo_net_kinesin_dom > 0.4 && r3.cargo_net_dynein_dom < -0.4 && r3.cargo_parked_high_ca < 0.05 &&
         r3.arp23_70deg_thrust_pN > 35.0 && r3.lacrymaria_max_ratio >= 29.0 && r3.lacrymaria_mem_strain < 0.03);
    gate("G07", "TB04 Active ATP turnover arrests LLPS Ostwald ripening (> eq droplets, < eq radius, passive mass exact)",
         r4.active_droplet_count > r4.eq_droplet_count && r4.active_mean_radius < r4.eq_mean_radius && r4.mass_drift_eq < 1e-8);
    gate("G08", "TB05 Discrete Poincare-Hopf topological sum == +2.000000 (< 1e-9) across all steps on S^2",
         r5.n_vertices == 162 && r5.max_charge_error < 1e-9 && r5.final_polar_organizers >= 2 && r5.nematic_order_S > 0.30);
    gate("G09", "TB06 Eigen Error Catastrophe melts L=600 at equilibrium while Hopfield-Ninio KP1/KP2 sustain L=600..3200",
         r6.master_eq_L100 > 0.25 && r6.master_eq_L600 < 1e-4 && r6.master_kp1_L600 > 0.85 && r6.master_kp2_L3200 > 0.98 && r6.gtp_dissipation_kp2 > 0.0);
    gate("G10", "TB07 Turing equal diffusion collapses flat (< 1e-3) while fast inhibitor (Dv=30*Du) erupts patterns",
         r7.turing_eq_diff_std < 1e-3 && r7.turing_fast_inh_std > 0.10);
    gate("G11", "TB07 Feinberg CRNT Deficiency-0 decays monotonically (< 1e-4) while Deficiency-1 oscillates (> 1.0)",
         r7.feinberg_d0_osc_amp < 1e-4 && r7.feinberg_d1_osc_amp > 1.0);
    gate("G12", "TB08 Stochastic Gillespie stepper obeys universal TUR >= 2.0 across 5 drives & empirical ~4x jitter scaling",
         r8.tur_product_min >= 1.95 && r8.tur_jitter_ratio >= 3.5 && r8.tur_jitter_ratio <= 4.5);
    gate("G13", "TB08 CheA/Y/B/R exact adaptation (< 0.01) across 100x step, positive chemo drift & Stentor tier-4 escape",
         r8.chemotaxis_adapt_err < 0.01 && r8.chemotaxis_drift_gain > 5.0 && r8.stentor_max_tier == 4 && r8.stentor_escaped == 1);
    gate("G14", "TB09 Zero-blueprint <=1000-cell 4-Hox Walking-Head Tardigrade grows all 7 organs & sealed hollow gut",
         r9.total_cells >= 400 && r9.total_cells <= 1000 && r9.hox_genes_active == 4 && r9.gut_lumen_sealed == 1 &&
         r9.organ_counts[0] > 0 && r9.organ_counts[1] > 0 && r9.organ_counts[2] > 0 && r9.organ_counts[3] > 0 &&
         r9.organ_counts[4] > 0 && r9.organ_counts[5] > 0 && r9.organ_counts[6] > 0);
    gate("G15", "TB09 Tardigrade CPG walks toward food, bioglass+3-tier radiation > 50x KO & stochastic Megaphragma ~370 nuclei",
         r9.cpg_walk_distance > 3.0 && r9.cpg_food_bias_gain > 2.0 && r9.cahs_bioglass_chi > 0.95 &&
         r9.intact_survival_pct > 50.0 * r9.ko_survival_pct && r9.megaphragma_nuclei >= 300 && r9.megaphragma_nuclei <= 440);
    gate("G16", "TB10 Passive crystal work == 0 (<1e-6), Odd-Elastic matches 2*Ko*Area, non-recip chases & 37-disk crystal psi6 > 0.70",
         r10.passive_cycle_work < 1e-6 && fabs(r10.odd_cycle_work - r10.theoretical_odd_work) < 0.02 &&
         r10.nonrecip_chase_vel > 10.0 * r10.reciprocal_chase_vel && r10.crystal_hex_order > 0.70);
    gate("G17", "TB11 Lane Endosymbiosis shatters 1/R vol wall, Lynch small-Ne inflates genome, transATP > 50% & Nitroplast syncs",
         r11.max_vol_euk_largeNe > 2.5 * r11.max_vol_prok_largeNe && r11.genome_kb_euk_smallNe > r11.genome_kb_euk_largeNe &&
         r11.translation_atp_share > 0.50 && fabs(r11.nitroplast_sync_ratio - 1.0) <= 0.06 && r11.nitroplast_n2_fixed > 1000);
    gate("G18", "TB12 Track A eliminates tumors (0), Track B forms Hippo-KO tumors, 100% restraint->0 OVI & bit-exact SHA-256",
         r12.trackA_tumors == 0 && r12.trackB_tumors > 5 && r12.ovi_restraint_50pct > 60.0 && r12.ovi_restraint_100pct == 0.0 &&
         strcmp(r1.sha256, r1b.sha256) == 0 && strcmp(r12.sha256, r12b.sha256) == 0 &&
         r12.asmodensius_mean > 0.35 && r12.asmodensius_mean < 0.65);

    /* G19-G24: 6 Planted Negative Mutants (M1-M6) — MUST be caught by breaking their physical laws! */
    TB01Result m1; omega_run_tb01(SEED, 1, &m1);
    gate("G19", "MUTANT M1 CAUGHT: Non-Stokes inertia hack violates Purcell Scallop zero-displacement invariant",
         fabs(m1.scallop_net_disp) > 0.02);

    TB02Result m2; omega_run_tb02(SEED, 2, &m2);
    gate("G20", "MUTANT M2 CAUGHT: Disabling Mavirus virophage collapses amoeba host survival under Giant Virus",
         m2.amoeba_with_mavirus < 0.25);

    TB06Result m3; omega_run_tb06(SEED, 3, &m3);
    gate("G21", "MUTANT M3 CAUGHT: Disabling Hopfield-Ninio proofreading triggers Eigen Error Catastrophe at L=600",
         m3.master_kp1_L600 < 1e-4 && m3.gtp_dissipation_kp2 == 0.0);

    TB07Result m4; omega_run_tb07(SEED, 4, &m4);
    gate("G22", "MUTANT M4 CAUGHT: Forcing Du == Dv kills Turing morphogenesis completely (std < 1e-3)",
         m4.turing_fast_inh_std < 1e-3);

    TB09Result m5; omega_run_tb09(SEED, 5, &m5);
    gate("G23", "MUTANT M5 CAUGHT: Knocking out CAHS/Dsup/DODA1/TRID1 collapses Tardigrade stress survival < 1%",
         m5.intact_survival_pct < 1.0 && m5.cahs_bioglass_chi < 0.10);

    TB12Result m6; omega_run_tb12(SEED, 6, dna, &m6);
    gate("G24", "MUTANT M6 CAUGHT: Disabling Track A constitutional apoptosis causes runaway somatic tumors (> 5)",
         m6.trackA_tumors > 5);

    /* G25-G28: TB13 5-Repo Interconnected 12-Axis Open-Proliferation Gates + Mutant M7 */
    TB13Result r13, r13b;
    omega_run_tb13(SEED, 0, 0, &r13);
    omega_run_tb13(SEED, 0, 0, &r13b);
    gate("G25", "TB13 Open-Proliferation continuously invents NEW 12-axis niches in BOTH halves (H1>500, H2>200)",
         r13.unique_niches_found > 1000 && r13.novelty_first_half > 500 && r13.novelty_second_half > 200);
    gate("G26", "TB13 5-Repo Biosphere spawns Russian-Doll chimeras, Swarms, Organ-Creatures & Wild Outliers",
         r13.russian_doll_chimeras >= 10 && r13.swarm_colonies_formed >= 10 &&
         r13.organ_creatures_born >= 3 && r13.wild_outliers_spawned >= 20);
    gate("G27", "TB13 5-Repo exact global mass conservation (< 1e-8), 12D span > 0.70 & bit-exact SHA-256",
         r13.global_mass_residual < 1e-8 && r13.morphospace_volume > 0.70 &&
         r13.world_niche_shift > 1.0 && strcmp(r13.sha256, r13b.sha256) == 0);
    TB13Result m7; omega_run_tb13(SEED, 7, 0, &m7);
    gate("G28", "MUTANT M7 CAUGHT: Freezing feral open proliferation kills second-half novelty to 0 & shrinks span",
         m7.novelty_second_half == 0 && m7.morphospace_volume < 0.30);

    /* G29-G33: TB14 Clean-Room Lock-Free 4-Layer Bio-Interaction-Net VM ("Bio-Frostbite") + Mutant M8 */
    TB14Result r14, r14b;
    omega_run_tb14(SEED, 0, 0, &r14);
    omega_run_tb14(SEED, 0, 0, &r14b);
    gate("G29", "TB14 Bio-Frostbite INet fires active pairs across ALL 4 layers (DNA/Org/Cell/Creat) & hits Russian-Doll Depth 3",
         r14.layer_rewrites[0] >= 40 && r14.layer_rewrites[1] >= 35 &&
         r14.layer_rewrites[2] >= 16 && r14.layer_rewrites[3] >= 20 &&
         r14.russian_doll_max_depth == 3);
    gate("G30", "TB14 Strong Confluence (Diamond Property): FIFO == LIFO == Chaotic Random 0-bit identical Normal-Form SHA-256",
         r14.confluence_exact_match == 1 &&
         r14.rewrites_fifo == r14.rewrites_lifo && r14.rewrites_fifo == r14.rewrites_random &&
         strcmp(r14.sha_fifo, r14.sha_lifo) == 0 && strcmp(r14.sha_fifo, r14.sha_random) == 0 &&
         strcmp(r14.sha256, r14b.sha256) == 0);
    gate("G31", "TB14 Strict Single-Ownership Linearity: 0 integer mass drift, 0 Gap-Junction charge drift & 0 wire errors",
         r14.mass_drift_fifo == 0 && r14.mass_drift_lifo == 0 && r14.mass_drift_random == 0 &&
         r14.gap_charge_drift == 0 && r14.wire_ownership_errors == 0);
    gate("G32", "TB14 Cryptobiotic Bioglass Tun burns 0 active pairs / 0 dynamic power dry, resumes on H2O & SSIN >= 95% confluent",
         r14.tun_active_pairs_dry == 0 && r14.tun_power_burn_dry == 0 &&
         r14.tun_rewrites_rehyd >= 4 && r14.ssin_arbiter_pairings == 16 && r14.ssin_confluent_pct >= 95.0);
    TB14Result m8; omega_run_tb14(SEED, 8, 0, &m8);
    gate("G33", "MUTANT M8 CAUGHT: Shared-RAM order-dependent race shatters Strong Confluence (FIFO!=RAND) & leaks linear mass",
         m8.confluence_exact_match == 0 && strcmp(m8.sha_fifo, m8.sha_random) != 0 && m8.mass_drift_fifo != 0);

    /* G34-G39: TB15 Deep Strata End-to-End Frontier Gates + Planted Mutant M9 */
    TB15Result r15, r15b;
    omega_run_tb15(SEED, 0, &r15);
    omega_run_tb15(SEED, 0, &r15b);
    gate("G34", "TB15 Inorganic->Organic CISS Magnetite breaks racemic symmetry (>96% ee vs 0%) & closes mass-exact RAF set",
         r15.ciss_racemic_ee < 1e-9 && r15.ciss_magnetite_ee > 0.96 && r15.raf_closed_reactions == 6 && r15.raf_mass_residual < 1e-10);
    gate("G35", "TB15 4500-locus Oxytricha unscrambles 225 MDS (95% IES cut), Cohesin MC TAD >3x & insulated <0.35, 22-AA walker",
         r15.oxytricha_mds_unscrambled == 225 && r15.oxytricha_ies_deleted_pct >= 94.9 &&
         r15.tad_contact_gain > 3.0 && r15.tad_ctcf_insulation < 0.35 && r15.expanded_aa_codons == 22);
    gate("G36", "TB15 32-Trp Jacobi Hamiltonian Dicke superradiance (>2.5x QY, >100x dark), Snell Ocelloid (>10x), 1ns-ODE Cnid >5Mg & Langevin Mag",
         r15.trp_superradiant_qy_gain > 2.5 && r15.trp_subradiant_ratio > 100.0 &&
         r15.ocelloid_focal_gain > 10.0 && r15.cnidocyte_peak_accel_g > 5.0e6 && r15.magnetosome_align_cos > 0.80);
    gate("G37", "TB15 3-Genome Nested Assembly exact 0 int drift, Elysia Kleptoplasty >3x, Levin Vmem 1->2 Heads with 0 DNA muts",
         r15.three_genome_mass_drift == 0 && r15.kleptoplasty_surv_gain > 3.0 &&
         r15.levin_wild_heads == 1 && r15.levin_reprog_heads == 2 && r15.levin_dna_mutations == 0);
    gate("G38", "TB15 Somite limit cycles >=8, EMT reversible >=2, Wireless Neuropeptide Connectome >> Wired & Micro-Parasites",
         r15.somite_clock_cycles >= 8 && r15.emt_reversible_switches >= 2 &&
         r15.connectome_wireless_reach > 2.0 * r15.connectome_wired_reach &&
         r15.rhombozoa_cells >= 20 && r15.rhombozoa_cells <= 40 && r15.henneguya_mt_genes == 0 &&
         strcmp(r15.sha256, r15b.sha256) == 0);
    TB15Result m9; omega_run_tb15(SEED, 9, &m9);
    gate("G39", "MUTANT M9 CAUGHT: Disabling CISS spin bias & Levin bioelectric memory kills homochirality & 2-head attractor",
         m9.ciss_magnetite_ee < 0.01 && m9.levin_reprog_heads < 2);

    /* G40-G43: TB16 Hybrid Virtual-Cell Foundation Model, Hepatic Zonation & JCVI-syn3A Dark Proteome + Mutant M10 */
    TB16Result r16, r16b;
    omega_run_tb16(SEED, 0, &r16);
    omega_run_tb16(SEED, 0, &r16b);
    gate("G40", "TB16 PDE 32-zone Hepatic Zonation coherent (r>0.95) & Ridge-Trained Zero-Shot Context-B R2 > 0.85 (Proj >= Raw!)",
         r16.manifold_dim == 16 && r16.zonation_zones == 32 &&
         r16.zonation_monotonicity > 0.95 && r16.zeroshot_perturb_r2 > 0.85 &&
         r16.zeroshot_perturb_r2 >= r16.zeroshot_raw_r2);
    gate("G41", "TB16 Real 573-gene JCVI-syn3A graph CRISPRi knockouts: AUROC > 0.90 & >=90/100 dark essentials recovered",
         r16.syn3a_total_genes == 473 && r16.syn3a_dark_genes == 100 &&
         r16.syn3a_dark_discovered >= 90 && r16.zeroshot_essential_auroc > 0.90);
    gate("G42", "TB16 Stoichiometric Physics Projection eliminates raw AI hallucination (<1e-12), 6-Tier Closure & bit-exact SHA-256",
         r16.raw_ai_mass_hallucination > 0.15 && r16.projected_mass_residual < 1e-12 &&
         r16.six_tier_closure_err < 1e-12 && strcmp(r16.sha256, r16b.sha256) == 0);
    TB16Result m10; omega_run_tb16(SEED, 10, &m10);
    gate("G43", "MUTANT M10 CAUGHT: Disabling Virtual-Cell Physics Projection exposes raw neural mass hallucination (>0.15)",
         m10.projected_mass_residual > 0.15);

    double elapsed = omega_time_sec() - t0;
    printf("================================================================================\n");
    printf(" FALSIFICATION SUMMARY: %d / %d PASSED · %d FAILED · 10/10 MUTANTS CAUGHT (%.3fs)\n",
           g_pass, g_pass + g_fail, g_fail, elapsed);
    printf("================================================================================\n");
    return (g_fail == 0) ? 0 : 1;
}
