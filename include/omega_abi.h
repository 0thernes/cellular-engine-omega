#ifndef OMEGA_ABI_H
#define OMEGA_ABI_H

#include <stdint.h>
#include <stddef.h>

#if defined(_WIN32) && defined(OMEGA_BUILD_DLL)
#define OMEGA_API __declspec(dllexport)
#else
#define OMEGA_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * Determinism, RNG & Hashing (P4 / P9 Invariants)
 * ============================================================================ */
typedef struct OmegaRng {
    uint64_t key;
    uint64_t ctr;
    uint32_t buf[4];
    int      idx;
} OmegaRng;

OMEGA_API void     omega_rng_seed(OmegaRng* r, uint64_t seed);
OMEGA_API uint32_t omega_rng_u32(OmegaRng* r);
OMEGA_API double   omega_rng_f64(OmegaRng* r);      /* [0, 1) */
OMEGA_API double   omega_rng_gauss(OmegaRng* r);    /* N(0, 1) */
OMEGA_API double   omega_time_sec(void);
OMEGA_API void     omega_sha256_hex(const void* data, size_t len, char out_hex[65]);

/* ============================================================================
 * 16 Experimental Testbed Result Structures (CELLULAR ENGINE OMEGA v5.0)
 * ============================================================================ */

/* TB01: Purcell Scallop Theorem & MotA5MotB2 Rotary Corkscrew */
typedef struct TB01Result {
    double scallop_net_disp;       /* 1-DOF reciprocal (must be ~0) */
    double scallop_peak_excursion; /* Peak mid-stroke excursion (> 0) */
    double purcell_2dof_disp;      /* 2-DOF non-reciprocal net displacement (> 0) */
    double purcell_loop_area;      /* Signed shape-space loop area (> 0) */
    double mota5_helical_disp;     /* MotA5MotB2 36-deg proton rotor net displacement */
    int    mota5_proton_steps;     /* Discrete 36-deg steps executed */
    double mutant_disp;            /* Mutant: non-Stokes inertia hack (> 0) */
    char   sha256[65];
} TB01Result;

/* TB02: Complete Virosphere (Phages, PhiKZ Nucleus, Giant Viruses, Mavirus, Obelisks) */
typedef struct TB02Result {
    double shannon_no_phage;       /* Monoculture collapse diversity */
    double shannon_with_phage;     /* Kill-the-Winner diversity (> no_phage) */
    double top_strain_no_phage;    /* Dominant strain share without phage */
    double top_strain_with_phage;  /* Dominant strain share with phage */
    double phikz_survival_no_shell;/* Phage survival vs CRISPR without chimallin */
    double phikz_survival_shell;   /* PhiKZ survival with chimallin phage nucleus */
    double amoeba_no_mavirus;      /* Host survival against Giant Virus without Mavirus */
    double amoeba_with_mavirus;    /* Host survival rescued by Mavirus virophage */
    int    orfan_genes_invented;   /* Pandoravirus/Tupanvirus de novo ORFans transferred */
    int    obelisk_colonized_cells;/* Active Obelisk circular-RNA host count */
    char   sha256[65];
} TB02Result;

/* TB03: Cytoskeleton (Dynamic Instability MT, Kinesin/Dynein Tug-of-War, Arp2/3, Lacrymaria) */
typedef struct TB03Result {
    double  centrosome_init_err;    /* Initial off-center error (um) */
    double  centrosome_final_err;   /* Final wall-force centered error (< init_err) */
    double  mt_mean_len_before_ca;  /* Mean MT length before Ca2+ spike */
    double  mt_mean_len_after_ca;   /* Mean MT length after Ca2+ melt (<< before) */
    int     mt_captured_count;      /* Rays selectively stabilized on cortical target */
    double  cargo_net_kinesin_dom;  /* Net (+) outward drift when Kinesin > Dynein */
    double  cargo_net_dynein_dom;   /* Net (-) inward drift when Dynein > Kinesin */
    double  cargo_parked_high_ca;   /* Miro/Ca2+ Hill-dissociation tug-of-war speed (~0) */
    double  arp23_70deg_thrust_pN;  /* Elastic Brownian ratchet thrust */
    double  lacrymaria_max_ratio;   /* Curved-crease origami neck extension ratio (~30x) */
    double  lacrymaria_mem_strain;  /* Peak membrane tear strain (< 0.05 safe) */
    float   centrosome_xy[2];       /* Exact final C11 centrosome (cx, cy) um */
    float   ray_len_before_ca[32];  /* Exact C11 32-ray lengths before Ca2+ spike */
    float   ray_len_after_ca[32];   /* Exact C11 32-ray lengths after Ca2+ spike */
    uint8_t ray_captured[32];       /* Exact C11 cortical capture flags per ray */
    char    sha256[65];
} TB03Result;

/* TB04: Active-Arrested LLPS Condensates (Cahn-Hilliard 32x32 PDE) */
typedef struct TB04Result {
    int    eq_droplet_count;       /* Equilibrium Ostwald-ripened droplet count (small) */
    double eq_mean_radius;         /* Equilibrium coarsened radius (large) */
    int    active_droplet_count;   /* ATP-arrested droplet count (> eq_droplet_count) */
    double active_mean_radius;     /* ATP-arrested finite radius (< eq_mean_radius) */
    double mass_drift_eq;          /* Mass conservation residual in passive Cahn-Hilliard */
    double mass_drift_active;      /* Mass conservation residual in active ATP Cahn-Hilliard */
    float  eq_field[1024];         /* 32x32 equilibrium snapshot for microscope */
    float  active_field[1024];     /* 32x32 active-arrested snapshot for microscope */
    char   sha256[65];
} TB04Result;

/* TB05: Poincare-Hopf Topological Nematic Defects on Closed Sphere (chi = +2) */
typedef struct TB05Result {
    int    n_vertices;             /* 162 geodesic sphere vertices */
    double init_charge_sum;        /* Initial sum of winding charges (must be +2.0) */
    double final_charge_sum;       /* Final sum of winding charges (must be +2.0) */
    int    init_half_defects;      /* Initial +1/2 comet defects */
    int    final_polar_organizers; /* Coalesced head/foot +1 (or paired +1/2) poles */
    double max_charge_error;       /* Max |sum(q) - 2.0| across entire simulation */
    double nematic_order_S;        /* Global surface nematic order parameter */
    double defect_coords[12];      /* Up to 4 top defect 3D positions (x,y,z) */
    char   sha256[65];
} TB05Result;

/* TB06: Eigen Quasispecies Error Threshold & Hopfield-Ninio Kinetic Proofreading */
typedef struct TB06Result {
    double master_eq_L100;         /* n=0 equilibrium master fraction at L=100 */
    double master_eq_L600;         /* n=0 equilibrium master fraction at L=600 (collapsed) */
    double master_kp1_L600;        /* n=1 proofreading master fraction at L=600 (alive!) */
    double master_kp2_L3200;       /* n=2 proofreading master fraction at L=3200 (alive!) */
    double gtp_dissipation_kp2;    /* Strictly positive proofreading entropy bill */
    double curve_L[7];             /* L values: 50,100,200,400,800,1600,3200 */
    double curve_eq[7];            /* Master seq survival n=0 */
    double curve_kp1[7];           /* Master seq survival n=1 */
    double curve_kp2[7];           /* Master seq survival n=2 */
    char   sha256[65];
} TB06Result;

/* TB07: Turing Instability (D_inh > D_act) & Feinberg CRNT Deficiency (delta=0 vs delta=1) */
typedef struct TB07Result {
    double turing_eq_diff_std;     /* Spatial std when Du == Dv (must collapse ~0) */
    double turing_fast_inh_std;    /* Spatial std when Dv = 20*Du (pattern emerges!) */
    double feinberg_d0_osc_amp;    /* Deficiency-0 network late oscillation amplitude (~0) */
    double feinberg_d1_osc_amp;    /* Deficiency-1 active network oscillation amplitude (>0) */
    float  turing_field[1024];     /* 32x32 Turing activator pattern for microscope */
    char   sha256[65];
} TB07Result;

/* TB08: TUR Precision-Cost Law, Berg-Purcell Bayesian Chemotaxis & Stentor Jennings Improv */
typedef struct TB08Result {
    double tur_product_min;        /* Min empirical (Var/Mean^2)*dS across all drives (>= 2.0) */
    double tur_drives[5];          /* 5 ATP affinity drives */
    double tur_products[5];        /* Measured TUR product at each drive */
    double tur_jitter_ratio;       /* Entropy ratio when halving jitter (~4x) */
    double chemotaxis_adapt_err;   /* Exact integral adaptation error across 100x jump */
    double chemotaxis_drift_gain;  /* Up-gradient drift improvement over random walk */
    int    stentor_max_tier;       /* Jennings escalation tier reached (4 = Detach/Escape) */
    int    stentor_escaped;        /* 1 if Stentor successfully escaped noxious stimulus */
    char   sha256[65];
} TB08Result;

/* TB09: Zero-Blueprint <=1000-Cell Walking-Head 4-Hox Tardigrade + CPG + Cryptobiosis + Megaphragma */
typedef struct TB09Result {
    int    total_cells;            /* Grown from 1 zygote; target <= 1000 (e.g., 780..960) */
    int    organ_counts[7];        /* Skin/Lobopod, Neuro, Muscle, Gut, Malpighian, Gonad, Storage */
    int    gut_lumen_sealed;       /* 1 if apical constriction sealed >=3 cells/ring hollow tube */
    int    hox_genes_active;       /* 4-Hox "Walking Head" compressed genome */
    int    axons_wired;            /* Netrin/Slit guided connections */
    int    axons_pruned;           /* Hebbian/STDP pruned redundant synapses */
    double cpg_walk_distance;      /* 8-lobopod Matsuoka CPG forward walk distance */
    double cpg_food_bias_gain;     /* Sensorimotor steering toward food plume */
    double cahs_bioglass_chi;      /* Vitrification order parameter during 98% drying */
    double intact_survival_pct;    /* Survival after 98% desiccation + 4000 Gy radiation */
    double ko_survival_pct;        /* Survival of CAHS/Dsup/DODA1/TRID1 knockout */
    int    megaphragma_neurons;    /* 7400 total neurons */
    int    megaphragma_nuclei;     /* 370 nucleated somata after 95% pupal lysis */
    char   sha256[65];
} TB09Result;

/* TB10: Odd Elasticity Living Chiral Crystals & Non-Reciprocal Chase Time Crystals */
typedef struct TB10Result {
    double passive_cycle_work;     /* Even crystal (K_odd = 0) closed-loop work (~0) */
    double odd_cycle_work;         /* Living chiral crystal (K_odd > 0) extracted work (>0) */
    double theoretical_odd_work;   /* Exact 2 * K_odd * Area prediction */
    double reciprocal_chase_vel;   /* Mean collective angular chase velocity when J_AB = J_BA (~0) */
    double nonrecip_chase_vel;     /* Persistent chiral chase velocity when J_AB = -J_BA (>0) */
    double crystal_hex_order;      /* Hexagonal bond-orientational order psi_6 */
    char   sha256[65];
} TB10Result;

/* TB11: Nick Lane 1/R Ceiling vs. Lynch-Marinov Population Drift 2x2 Lab + Nitroplast */
typedef struct TB11Result {
    double max_vol_prok_largeNe;   /* Endo OFF, Ne=400 */
    double max_vol_prok_smallNe;   /* Endo OFF, Ne=40  */
    double max_vol_euk_largeNe;    /* Endo ON,  Ne=400 (shatters 1/R wall!) */
    double max_vol_euk_smallNe;    /* Endo ON,  Ne=40  (shatters 1/R + drift genome bloat!) */
    double genome_kb_euk_largeNe;  /* Compact streamlined eukaryote genome */
    double genome_kb_euk_smallNe;  /* Lynch-Marinov drift-expanded genome (> largeNe) */
    double translation_atp_share;  /* Empirical protein translation ATP fraction (> 0.50) */
    double nitroplast_sync_ratio;  /* UCYN-A division synchronization with algal host (~1.0) */
    int    nitroplast_n2_fixed;    /* Fixed NH3 units supplied to host */
    char   sha256[65];
} TB11Result;

/* TB12: Track A Apoptosis vs. Track B Hippo-YAP Cancer + 13-Vector Omega Lab + 4-Build Bridge */
typedef struct TB12Result {
    int    trackA_tumors;          /* Constitutional apoptosis tumors (must be 0) */
    int    trackB_tumors;          /* Feral Hippo-YAP KO spontaneous tumors (> 0) */
    double ovi_feral_baseline;     /* Unconstrained Omega Vitality Index */
    double ovi_restraint_50pct;    /* Partial 50% restraint (Escape-Breeding Paradox!) */
    double ovi_restraint_100pct;   /* Total 100% castration collapse (must be ~0) */
    int    escape_mutations_50pct; /* Kill-switch deletions + dormancy + HGT smuggles */
    float  asmodensius_112[112];   /* Canonical Build-3 [0,1]^112 projected trait vector */
    double asmodensius_mean;
    double asmodensius_std;
    char   sha256[65];
} TB12Result;

/* TB13: 5-Repo Interconnected 12-Axis Continuous Morphospace & Feral Open-Proliferation Engine */
typedef struct TB13Result {
    int    epochs_run;             /* Simulation epochs across 5 interconnected domes */
    int    active_lineages;        /* Living lineages across all biomes */
    int    unique_niches_found;    /* Cumulative distinct 12-axis morphospace hypercells discovered */
    int    novelty_first_half;     /* Niches discovered in first half (open proliferation check) */
    int    novelty_second_half;    /* NEW niches still discovered in second half (> 0!) */
    int    russian_doll_chimeras;  /* Depth 2-3 nested endosymbionts / 4-genome nucleomorphs */
    int    swarm_colonies_formed;  /* Cable-wire / Rosette / Dictyostelium / Symbiont collectives */
    int    organ_creatures_born;   /* Lineages crossing into multicellular organ-tissue creatures */
    int    wild_outliers_spawned;  /* Extreme outliers (0-nuc, 64N, Ocelloid, 80atm, Cable, Radiotroph) */
    double morphospace_volume;     /* Hyper-volume / std spread across all 12 continuous axes */
    double world_niche_shift;      /* Magnitude of reciprocal dome environmental modification */
    double repo_5layer_flux;       /* Cross-layer DNA<->Cell<->Creature<->Swarm<->World coupling */
    double global_mass_residual;   /* Exact 5-repo conservation closure error (must be ~0) */
    double axis_means[12];         /* Population mean across all 12 continuous cellular axes */
    double axis_spans[12];         /* Population max-min span across all 12 axes */
    char   apex_archetype[64];     /* Emergent phenotypic descriptor of wild apex lineage */
    char   sha256[65];
} TB13Result;

/* TB14: Clean-Room Lock-Free 4-Layer Lafont Bio-Interaction-Net VM ("Bio-Frostbite") + Hybrid SSIN */
typedef struct TB14Result {
    int      total_nodes_init;       /* Initial 4-layer graph node count */
    int      rewrites_fifo;          /* Active-pair rewrites to normal form (Schedule A: Forward FIFO) */
    int      rewrites_lifo;          /* Active-pair rewrites to normal form (Schedule B: Reverse LIFO) */
    int      rewrites_random;        /* Active-pair rewrites to normal form (Schedule C: Chaotic Random Parallel) */
    int      layer_rewrites[4];      /* Active-pair rewrites by layer: [0]=DNA, [1]=Organelle, [2]=Cell, [3]=Creature */
    int      confluence_exact_match; /* 1 iff sha_fifo == sha_lifo == sha_random (0-bit identical!) */
    int64_t  init_mass_q;            /* Initial exact integer linear mass quantum across all 4 layers */
    int64_t  init_charge_q;          /* Initial exact integer bioelectric charge quantum across all 4 layers */
    int64_t  mass_drift_fifo;        /* Exact integer linear mass residual (must be 0) */
    int64_t  mass_drift_lifo;        /* Exact integer linear mass residual (must be 0) */
    int64_t  mass_drift_random;      /* Exact integer linear mass residual (must be 0) */
    int64_t  gap_charge_drift;       /* Exact Gap-Junction bioelectric Vm charge residual (must be 0) */
    int      wire_ownership_errors;  /* Single-ownership symmetric port audit errors (must be 0) */
    int      russian_doll_max_depth; /* Maximum nested Membrane Box depth reached (3 = Mealybug/Tremblaya/Moranella) */
    int      tun_active_pairs_dry;   /* Active pairs fired during CAHS Bioglass desiccation (must be 0!) */
    int64_t  tun_power_burn_dry;     /* Dynamic power units burned during cryptobiotic tun (must be 0!) */
    int      tun_rewrites_rehyd;     /* Active pairs resumed cleanly after H2O rehydration (> 0) */
    int      ssin_arbiter_pairings;  /* Spatial Voxel_Arbiter disjoint pairings (2k <= m) */
    int      ssin_agents_contended;  /* Total multi-agent 3D voxel contenders arbitrated (m >= 2k) */
    int      ssin_confluent_rewrites;/* Pure confluent lock-free internal rewrites (95%+ compute!) */
    int      causal_4layer_hops;     /* End-to-end L1(DNA)->L2(Org)->L3(Cell)->L4(Creat) causal hops */
    double   ssin_confluent_pct;     /* Confluent parallel silicon compute percentage (>= 95%) */
    char     sha_fifo[65];           /* Normal-form canonical SHA-256 (Schedule A: Forward FIFO) */
    char     sha_lifo[65];           /* Normal-form canonical SHA-256 (Schedule B: Reverse LIFO) */
    char     sha_random[65];         /* Normal-form canonical SHA-256 (Schedule C: Chaotic Random Parallel) */
    char     sha256[65];             /* Bit-exact deterministic P4/P9 master receipt (excludes wall clock!) */
    double   rewrites_per_sec;       /* Wall-clock telemetry (placed AFTER sha256 so receipt is bit-exact!) */
} TB14Result;

/* TB15: Deep Strata End-to-End Frontier (Inorganic CISS -> TAD/Oxytricha -> Trp Superradiance -> Levin Vmem -> Wireless Connectome) */
typedef struct TB15Result {
    double   ciss_racemic_ee;           /* Abiotic racemic baseline (~0.00) */
    double   ciss_magnetite_ee;         /* UV Magnetite CISS + conglomerate homochiral ee (> 0.96) */
    int      raf_closed_reactions;      /* Autocatalytic Food-generated RAF set reactions */
    double   raf_mass_residual;         /* Exact prebiotic RAF mass residual (< 1e-10) */
    int      oxytricha_mds_unscrambled; /* Scrambled germline MDS exons assembled into somatic MAC */
    double   oxytricha_ies_deleted_pct; /* Non-coding IES junk excised (~95%) */
    double   tad_contact_gain;          /* Cohesin loop-extrusion intra-TAD enhancer contact boost (> 5x) */
    double   tad_ctcf_insulation;       /* Cross-CTCF barrier leakage (< 0.20) */
    int      expanded_aa_codons;        /* Expanded genetic alphabet (22 = 20 + Sec + Pyl) */
    double   trp_superradiant_qy_gain;  /* Microtubule Trp mega-network Dicke UV QY boost (> 2.5x) */
    double   trp_subradiant_ratio;      /* Subradiant dark-state coherence buffer ratio (> 100x) */
    double   ocelloid_focal_gain;       /* Warnowiid single-cell camera-eye refractive gain (> 10x) */
    double   cnidocyte_peak_accel_g;    /* 700 ns osmotic nematocyst harpoon acceleration (> 5e6 g) */
    double   magnetosome_align_cos;     /* Fe3O4 magnetosome geomagnetic alignment cos(theta) (> 0.80) */
    int64_t  three_genome_mass_drift;   /* Mealybug/Tremblaya/Moranella 3-genome exact int drift (0) */
    double   kleptoplasty_surv_gain;    /* Elysia stolen-chloroplast starvation survival gain (> 3x) */
    int      levin_wild_heads;          /* Wild bioelectric planarian polarity (1 head) */
    int      levin_reprog_heads;        /* Transient Vmem pulse heritable attractor across amputations (2 heads!) */
    int      levin_dna_mutations;       /* Exact genetic mutations introduced during Vmem reprogram (0!) */
    float    levin_vmem_wild[32];       /* 32-cell anterior->posterior Vmem profile (mV, wild) */
    float    levin_vmem_2head[32];      /* 32-cell anterior->posterior Vmem profile (mV, bicephalic) */
    int      somite_clock_cycles;       /* Vertebrate Hes/Her segmentation-clock limit cycles */
    int      emt_reversible_switches;   /* Epithelial <-> Mesenchymal reversible state transitions */
    double   connectome_wired_reach;    /* C. elegans 302-neuron wired synaptic 2-hop reachability */
    double   connectome_wireless_reach; /* Wired + wireless neuropeptide reachability (> wired!) */
    int      rhombozoa_cells;           /* Eutelic cleavage mesozoan micro-parasite cell count (20..40) */
    int      henneguya_mt_genes;        /* Anoxic gene-loss cnidarian micro-parasite mtDNA genes (0) */
    int      cohesin_steric_blocks;     /* Excluded-volume Cohesin-Cohesin lattice collision stalls */
    int      three_gen_ko_blocked;      /* Single-genome KOs blocking 3-genome pathway closure (must be 3/3) */
    float    cnidocyte_accel_curve[32]; /* 32-pt real 1ns-ODE acceleration trajectory (units of 10^6 g) */
    float    tad_contact_profile[32];   /* 32-bin real Cohesin/CTCF loop-extrusion contact curve across CTCF */
    char     sha256[65];
} TB15Result;

/* TB16: Hybrid Virtual-Cell Foundation Model, Continuous Hepatic Zonation & JCVI-syn3A Dark Proteome */
typedef struct TB16Result {
    int      manifold_dim;              /* Continuous Waddington/transcriptomic latent dimensions (16) */
    int      zonation_zones;            /* Continuous Portal->Central hepatic zonation bands (32) */
    float    zonation_o2[32];           /* PDE-solved continuous O2 gradient along hepatic sinusoid */
    float    zonation_gluconeo[32];     /* PDE/Hill periportal gluconeogenesis continuous profile */
    float    zonation_glycolysis[32];   /* PDE/Hill pericentral glycolysis/CYP450 continuous profile */
    double   zonation_monotonicity;     /* Gradient correlation coherence (> 0.95) */
    double   zeroshot_raw_r2;           /* Unprojected Ridge-trained AI held-out Context B R^2 */
    double   zeroshot_perturb_r2;       /* Stoichiometric-projected Ridge-trained AI Context B R^2 (> 0.85) */
    double   zeroshot_essential_auroc;  /* Real 593-gene JCVI-syn3A graph CRISPRi knockout AUROC (> 0.90) */
    int      syn3a_total_genes;         /* JCVI-syn3A minimal genome size (493: 401 characterized + 92 dark) */
    int      syn3a_dark_genes;          /* Essential unknown-function "dark proteome" genes (92) */
    int      syn3a_dark_discovered;     /* Essential dark genes recovered via real flux-collapse knockouts */
    double   raw_ai_mass_hallucination; /* Unconstrained Ridge-fitted prediction mass drift (> 0.15) */
    double   projected_mass_residual;   /* Physics-projected stoichiometric residual (< 1e-12; broken by M10!) */
    double   six_tier_closure_err;      /* Coupled 6-tier Inorganic->VirtualCell conservation residual */
    int64_t  six_tier_int_drift;        /* Exact integer 6-tier closure ledger drift (must be 0 for O63!) */
    float    syn3a_ko_flux_hist[32];    /* Real 32-bin CRISPRi knockout flux-collapse histogram (dark vs ctrl) */
    char     sha256[65];
} TB16Result;

/* Cross-Testbed Live 6-Tier Conservation Ledger fed into TB16 (O58 / O63) */
typedef struct OmegaTierLedger {
    double  t1_raf_mass_residual;       /* Tier 1: Inorganic->Prebiotic RAF (TB15) */
    double  t1_raf_total_mass;          /* Tier 1 reference mass */
    int64_t t2_three_genome_mass_drift; /* Tier 2: 3-Genome Molecular Assembly (TB15) */
    double  t3_llps_mass_drift;         /* Tier 3: Organelle LLPS Cahn-Hilliard (TB04) */
    double  t3_llps_total_mass;         /* Tier 3 reference mass */
    int64_t t4_inet_mass_drift;         /* Tier 4: Cellular 4-Layer Bio-Frostbite INet (TB14) */
    int64_t t4_inet_charge_drift;       /* Tier 4: Bio-Frostbite Gap-Junction charge drift (TB14) */
    double  t5_biosphere_mass_residual; /* Tier 5: 5-Repo Biosphere & Topology (TB13) */
    double  t5_biosphere_total_mass;    /* Tier 5 reference mass */
    double  t5_topology_charge_err;     /* Tier 5: Poincare-Hopf S^2 charge err (TB05) */
} OmegaTierLedger;

/* Public Runner Entry Points */
OMEGA_API void omega_run_tb01(uint64_t seed, int mutant_mode, TB01Result* out);
OMEGA_API void omega_run_tb02(uint64_t seed, int mutant_mode, TB02Result* out);
OMEGA_API void omega_run_tb03(uint64_t seed, int mutant_mode, TB03Result* out);
OMEGA_API void omega_run_tb04(uint64_t seed, int mutant_mode, TB04Result* out);
OMEGA_API void omega_run_tb05(uint64_t seed, int mutant_mode, TB05Result* out);
OMEGA_API void omega_run_tb06(uint64_t seed, int mutant_mode, TB06Result* out);
OMEGA_API void omega_run_tb07(uint64_t seed, int mutant_mode, TB07Result* out);
OMEGA_API void omega_run_tb08(uint64_t seed, int mutant_mode, TB08Result* out);
OMEGA_API void omega_run_tb09(uint64_t seed, int mutant_mode, TB09Result* out);
OMEGA_API void omega_run_tb10(uint64_t seed, int mutant_mode, TB10Result* out);
OMEGA_API void omega_run_tb11(uint64_t seed, int mutant_mode, TB11Result* out);
OMEGA_API void omega_run_tb12(uint64_t seed, int mutant_mode, const uint8_t autophage_dna[256], TB12Result* out);
OMEGA_API void omega_run_tb13(uint64_t seed, int mutant_mode, int feral_hw_entropy, TB13Result* out);
OMEGA_API void omega_run_tb14(uint64_t seed, int mutant_mode, int feral_hw_entropy, TB14Result* out);
OMEGA_API void omega_run_tb15(uint64_t seed, int mutant_mode, TB15Result* out);
OMEGA_API void omega_run_tb16(uint64_t seed, int mutant_mode, TB16Result* out);
OMEGA_API void omega_run_tb16_coupled(uint64_t seed, int mutant_mode, const OmegaTierLedger* ledger, TB16Result* out);

#ifdef __cplusplus
}
#endif

#endif /* OMEGA_ABI_H */
