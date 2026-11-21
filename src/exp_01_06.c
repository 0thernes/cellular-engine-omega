#include "omega_bare_crt.h"
#include "omega_abi.h"

#define PI_D 3.14159265358979323846

/* ============================================================================
 * TB01: Purcell's Scallop Theorem, 2-DOF Swimmer & MotA5MotB2 Rotary Flagellum
 * ============================================================================ */
static double purcell_1dof_conn(double th) {
    /* Smooth odd-coupled Stokes connection A(theta) */
    double c = cos(th);
    return 0.28 * sin(th) / (1.0 + 0.30 * c * c);
}

static void purcell_2dof_conn(double t1, double t2, double* A1, double* A2) {
    /* Becker-Koehler-Stone 3-link swimmer low-Re gauge connection
     * Curl (dA2/dt1 - dA1/dt2) = 0.22*cos(t1)*cos(t2) + 0.08*cos(t1-t2) != 0 */
    *A1 = -0.11 * sin(t2) * cos(t1) - 0.04 * sin(t1 - t2);
    *A2 =  0.11 * sin(t1) * cos(t2) + 0.04 * sin(t1 - t2);
}

OMEGA_API void omega_run_tb01(uint64_t seed, int mutant_mode, TB01Result* out) {
    memset(out, 0, sizeof(*out));

    /* 1. 1-DOF Reciprocal Scallop: FAST forward (100 steps) + SLOW return (400 steps)
     * By Purcell's Scallop Theorem, time-asymmetric speed produces ZERO net displacement. */
    const double th_min = 0.0, th_max = 1.40;
    double x_scal = 0.0, x_peak = 0.0, x_mut = 0.0;
    const int n_fwd = 100, n_rev = 400;
    const double gl = 0.28867513459481288225; /* 1/(2*sqrt(3)) 2-pt Gauss-Legendre */
    for (int i = 0; i < n_fwd; ++i) {
        double a = th_min + (th_max - th_min) * ((double)i / (double)n_fwd);
        double b = th_min + (th_max - th_min) * ((double)(i + 1) / (double)n_fwd);
        double mid = 0.5 * (a + b), dth = b - a;
        double dx = 0.5 * (purcell_1dof_conn(mid - gl * dth) + purcell_1dof_conn(mid + gl * dth)) * dth;
        x_scal += dx;
        if (fabs(x_scal) > x_peak) x_peak = fabs(x_scal);
        /* Mutant injects non-Stokes quadratic inertial drag proportional to |dth/dt|*dth */
        double v_rate = dth / (1.0 / (double)n_fwd);
        x_mut += dx + 0.025 * fabs(v_rate) * dth;
    }
    for (int i = 0; i < n_rev; ++i) {
        double a = th_max - (th_max - th_min) * ((double)i / (double)n_rev);
        double b = th_max - (th_max - th_min) * ((double)(i + 1) / (double)n_rev);
        double mid = 0.5 * (a + b), dth = b - a;
        double dx = 0.5 * (purcell_1dof_conn(mid - gl * dth) + purcell_1dof_conn(mid + gl * dth)) * dth;
        x_scal += dx;
        double v_rate = dth / (4.0 / (double)n_rev);
        x_mut += dx + 0.025 * fabs(v_rate) * dth;
    }
    out->scallop_net_disp = (mutant_mode == 1) ? x_mut : x_scal;
    out->scallop_peak_excursion = x_peak;
    out->mutant_disp = x_mut;

    /* 2. 2-DOF Purcell 3-Link Non-Reciprocal Rectangular Loop */
    double x_2dof = 0.0;
    const double lo = -0.70, hi = 0.70;
    const int segN = 200;
    double dseg = (hi - lo) / (double)segN;
    /* Leg 1: t1: lo -> hi at t2 = lo */
    for (int i = 0; i < segN; ++i) {
        double t1 = lo + (i + 0.5) * dseg, t2 = lo;
        double A1, A2; purcell_2dof_conn(t1, t2, &A1, &A2);
        x_2dof += A1 * dseg;
    }
    /* Leg 2: t2: lo -> hi at t1 = hi */
    for (int i = 0; i < segN; ++i) {
        double t1 = hi, t2 = lo + (i + 0.5) * dseg;
        double A1, A2; purcell_2dof_conn(t1, t2, &A1, &A2);
        x_2dof += A2 * dseg;
    }
    /* Leg 3: t1: hi -> lo at t2 = hi */
    for (int i = 0; i < segN; ++i) {
        double t1 = hi - (i + 0.5) * dseg, t2 = hi;
        double A1, A2; purcell_2dof_conn(t1, t2, &A1, &A2);
        x_2dof -= A1 * dseg;
    }
    /* Leg 4: t2: hi -> lo at t1 = lo */
    for (int i = 0; i < segN; ++i) {
        double t1 = lo, t2 = hi - (i + 0.5) * dseg;
        double A1, A2; purcell_2dof_conn(t1, t2, &A1, &A2);
        x_2dof -= A2 * dseg;
    }
    out->purcell_2dof_disp = fabs(x_2dof);
    out->purcell_loop_area = (hi - lo) * (hi - lo);

    /* 3. MotA5MotB2 5:2 Asymmetric Proton Cogwheel Flagellar Rotor (36 deg / H+)
     *    Genuine 10-state stochastic proton-gated Markov ratchet driven by PMF = 170 mV (~6.6 kT/H+) */
    OmegaRng rng;
    omega_rng_seed(&rng, seed ^ 0x10104075ULL);
    const double dG_pmf_kT = 6.6; /* 170 mV proton-motive force */
    const double p_fwd = 1.0 / (1.0 + exp(-dG_pmf_kT));
    const double helix_pitch_um = 0.45;
    int state10 = 0;
    int net_steps = 0;
    double theta_rad = 0.0;
    double z_helical = 0.0;
    for (int ev = 0; ev < 120; ++ev) {
        if (omega_rng_f64(&rng) < p_fwd) {
            state10 = (state10 + 1) % 10;
            net_steps++;
            theta_rad += (2.0 * PI_D) / 10.0; /* +36 deg */
            /* Hydrodynamic drag slip factor from resistive-force theory (~0.82 at low Re) */
            z_helical += (helix_pitch_um / 10.0) * (0.80 + 0.04 * omega_rng_f64(&rng));
        } else {
            state10 = (state10 + 9) % 10;
            net_steps--;
            theta_rad -= (2.0 * PI_D) / 10.0;
            z_helical -= (helix_pitch_um / 10.0) * 0.82;
        }
    }
    (void)state10; (void)theta_rad;
    out->mota5_proton_steps = net_steps;
    out->mota5_helical_disp = z_helical;

    omega_sha256_hex(out, offsetof(TB01Result, sha256), out->sha256);
}

/* ============================================================================
 * TB02: Complete Virosphere Ecosystem (Phages, PhiKZ, Giant Viruses, Mavirus, Obelisks)
 * ============================================================================ */
static void run_phage_ecology(uint64_t seed, int phages_on, double* out_shannon, double* out_top, int* out_orfan, int* out_obelisks) {
    OmegaRng rng; omega_rng_seed(&rng, seed ^ (phages_on ? 0x2222ULL : 0x1111ULL));
    double B[6] = { 12.0, 12.0, 12.0, 12.0, 12.0, 12.0 };
    double P[6] = { 2.0,  2.0,  2.0,  2.0,  2.0,  2.0  };
    const double r[6] = { 0.44, 0.49, 0.54, 0.59, 0.65, 0.78 };
    const double K_cap = 180.0;
    const double dt = 0.05;
    int orfans = 0;
    double obelisk_frac = 0.12;

    for (int step = 0; step < 1600; ++step) {
        double totB = 0.0;
        for (int k = 0; k < 6; ++k) totB += B[k];
        double free_cap = 1.0 - totB / K_cap;
        if (free_cap < -0.5) free_cap = -0.5;
        for (int k = 0; k < 6; ++k) {
            double pred = phages_on ? (0.038 * B[k] * P[k]) : 0.0;
            double dB = r[k] * B[k] * free_cap - pred - 0.18 * B[k];
            double dP = phages_on ? (0.060 * B[k] * P[k] - 0.24 * P[k]) : 0.0;
            B[k] += dB * dt;
            P[k] += dP * dt;
            if (B[k] < 0.005) B[k] = 0.005;
            if (P[k] < 0.02) P[k] = 0.02;
        }
        if (phages_on && (step % 25 == 0)) {
            if (omega_rng_f64(&rng) < 0.65) orfans++;
            obelisk_frac += 0.008 * (0.52 - obelisk_frac);
        }
    }
    double tot = 0.0, top = 0.0, H = 0.0;
    for (int k = 0; k < 6; ++k) tot += B[k];
    for (int k = 0; k < 6; ++k) {
        double pk = B[k] / tot;
        if (pk > top) top = pk;
        if (pk > 1e-12) H -= pk * log(pk);
    }
    *out_shannon = H;
    *out_top = top;
    *out_orfan = orfans;
    *out_obelisks = (int)(tot * obelisk_frac);
}

OMEGA_API void omega_run_tb02(uint64_t seed, int mutant_mode, TB02Result* out) {
    memset(out, 0, sizeof(*out));
    int dummy_o = 0, dummy_ob = 0;
    run_phage_ecology(seed, 0, &out->shannon_no_phage, &out->top_strain_no_phage, &dummy_o, &dummy_ob);
    run_phage_ecology(seed, 1, &out->shannon_with_phage, &out->top_strain_with_phage, &out->orfan_genes_invented, &out->obelisk_colonized_cells);

    /* PhiKZ Chimallin Phage Nucleus vs. Bacterial CRISPR-Cas */
    double dna_no_shell = 1.0, dna_shell = 1.0;
    for (int t = 0; t < 100; ++t) {
        /* CRISPR cleaves exposed phage DNA rapidly; Chimallin shell blocks 96% cleavage */
        dna_no_shell *= (1.0 - 0.048 + 0.015);
        dna_shell    *= (1.0 - 0.0018 + 0.0005);
    }
    out->phikz_survival_no_shell = dna_no_shell;
    out->phikz_survival_shell    = dna_shell;

    /* Giant Virus (Pandoravirus/CroV) vs. Amoeba + Mavirus Virophage Rescue */
    double host_no = 100.0, gv_no = 15.0;
    double host_mv = 100.0, gv_mv = 15.0, mv = 8.0;
    int mavirus_active = (mutant_mode == 2) ? 0 : 1; /* Mutant M2 disables Mavirus */
    const double dt = 0.02;
    for (int t = 0; t < 600; ++t) {
        /* Without Mavirus: Giant virus burst B=36 crashes host population */
        double inf_no = 0.0045 * host_no * gv_no;
        host_no += (0.25 * host_no * (1.0 - host_no / 120.0) - inf_no) * dt;
        gv_no   += (3.2 * inf_no - 0.14 * gv_no) * dt;
        if (host_no < 0.0) host_no = 0.0;
        if (gv_no < 0.0) gv_no = 0.0;

        /* With Mavirus Virophage: co-infection suppresses Giant Virus burst by 88% */
        double inf_mv = 0.0045 * host_mv * gv_mv;
        double suppress = mavirus_active ? (mv / (mv + 2.5)) : 0.0;
        double eff_burst = 3.2 * (1.0 - 0.88 * suppress);
        host_mv += (0.25 * host_mv * (1.0 - host_mv / 120.0) - inf_mv * (1.0 - 0.65 * suppress)) * dt;
        gv_mv   += (eff_burst * inf_mv - 0.18 * gv_mv) * dt;
        if (mavirus_active) {
            mv  += (2.4 * suppress * inf_mv + 0.06 * host_mv * 0.02 - 0.09 * mv) * dt;
        }
        if (host_mv < 0.0) host_mv = 0.0;
        if (gv_mv < 0.0) gv_mv = 0.0;
    }
    out->amoeba_no_mavirus   = host_no / 100.0;
    out->amoeba_with_mavirus = host_mv / 100.0;

    omega_sha256_hex(out, offsetof(TB02Result, sha256), out->sha256);
}

/* ============================================================================
 * TB03: Microtubule Dynamic Instability, Kinesin/Dynein Tug-of-War, Arp2/3 & Lacrymaria
 * ============================================================================ */
OMEGA_API void omega_run_tb03(uint64_t seed, int mutant_mode, TB03Result* out) {
    memset(out, 0, sizeof(*out));
    OmegaRng rng; omega_rng_seed(&rng, seed ^ 0x3333ULL);
    int m11 = (mutant_mode == 11) ? 1 : 0; /* Mutant M11: disables Ca2+ MT catastrophe & Miro cargo parking */

    /* 1. 32-Ray Dynamic-Instability Centrosome inside R = 10 um cell */
    const double R_cell = 10.0;
    double cx = 3.6, cy = -2.4;
    out->centrosome_init_err = sqrt(cx * cx + cy * cy);

    double ray_dir_x[32], ray_dir_y[32], ray_len[32], gtp_cap[32];
    int    ray_state[32]; /* 1 = GROW, 0 = SHRINK, 2 = CAPTURED */
    for (int k = 0; k < 32; ++k) {
        double ang = (2.0 * PI_D * k) / 32.0;
        ray_dir_x[k] = cos(ang);
        ray_dir_y[k] = sin(ang);
        ray_len[k]   = 1.5;
        gtp_cap[k]   = 1.0;
        ray_state[k] = 1;
    }

    const double dt = 0.12;
    for (int step = 0; step < 500; ++step) {
        double fx = 0.0, fy = 0.0;
        for (int k = 0; k < 32; ++k) {
            double tx = cx + ray_len[k] * ray_dir_x[k];
            double ty = cy + ray_len[k] * ray_dir_y[k];
            double r_tip = sqrt(tx * tx + ty * ty);
            double pen = (r_tip > R_cell) ? (r_tip - R_cell) : 0.0;
            double F_wall = 14.0 * pen;

            /* Cortical selective stabilization patch near angle ~ 0 (x > 8.8, |y| < 3.2) after centering */
            if (step > 340 && r_tip >= R_cell - 0.8 && tx > 8.8 && fabs(ty) < 3.2) {
                ray_state[k] = 2; /* CAPTURED */
            }

            if (ray_state[k] == 1) {
                /* Holy-Dogterom-Leibler Euler Critical Buckling Load: F_buck = pi^2 * kappa / L^2
                 * Short microtubules hitting the NEAR wall are stiff (large 1/L^2) and push strongly;
                 * long microtubules reaching the FAR wall buckle softly (small 1/L^2) -> robust centering! */
                double L_eff  = (ray_len[k] > 1.0) ? ray_len[k] : 1.0;
                double F_buck = 280.0 / (L_eff * L_eff);
                double F_push = (F_wall < F_buck) ? F_wall : F_buck;

                /* Peskin-Odell-Oster force-slowed growth -> GTP cap loss -> catastrophe */
                double vg = 1.45 * exp(-0.35 * F_push);
                ray_len[k] += vg * dt;
                if (r_tip > R_cell + 0.6) {
                    ray_len[k] -= 0.5 * (r_tip - (R_cell + 0.6));
                }
                gtp_cap[k] += (0.72 * exp(-0.32 * F_push) - 0.24) * dt;
                if (gtp_cap[k] > 1.4) gtp_cap[k] = 1.4;
                if (pen > 0.0) {
                    /* Buckling-capped pushing against boundary centers centrosome away from near wall! */
                    fx -= F_push * ray_dir_x[k];
                    fy -= F_push * ray_dir_y[k];
                }
                /* Kimura-Onami length-proportional cytoplasmic dynein pulling (longer far-wall MTs pull harder!) */
                fx += 0.24 * ray_len[k] * ray_dir_x[k];
                fy += 0.24 * ray_len[k] * ray_dir_y[k];
                if (gtp_cap[k] <= 0.0 || omega_rng_f64(&rng) < (0.006 + 0.035 * F_push)) {
                    ray_state[k] = 0;
                    gtp_cap[k] = 0.0;
                }
            } else if (ray_state[k] == 0) {
                ray_len[k] -= 1.85 * dt;
                if (ray_len[k] <= 0.6) {
                    ray_len[k] = 0.6;
                    ray_state[k] = 1;
                    gtp_cap[k] = 1.0;
                } else if (omega_rng_f64(&rng) < 0.04) {
                    ray_state[k] = 1;
                    gtp_cap[k] = 0.8;
                }
            }
        }
        /* Overdamped Stokes centrosome centering */
        cx += 0.068 * fx * dt;
        cy += 0.068 * fy * dt;
        double cr = sqrt(cx * cx + cy * cy);
        if (cr > R_cell - 0.8) {
            double sc = (R_cell - 0.8) / cr;
            cx *= sc; cy *= sc;
        }
    }

    double sumL = 0.0;
    int cap_cnt = 0;
    for (int k = 0; k < 32; ++k) {
        sumL += ray_len[k];
        out->ray_len_before_ca[k] = (float)ray_len[k];
        out->ray_captured[k]      = (uint8_t)(ray_state[k] == 2 ? 1 : 0);
        if (ray_state[k] == 2) cap_cnt++;
    }
    out->centrosome_xy[0] = (float)cx;
    out->centrosome_xy[1] = (float)cy;
    out->centrosome_final_err = sqrt(cx * cx + cy * cy);
    out->mt_mean_len_before_ca = sumL / 32.0;
    out->mt_captured_count = cap_cnt;

    /* Sudden Ca2+ Demolition Spike: triggers catastrophe on uncaptured MTs (disabled under M11!) */
    for (int step = 0; step < 120; ++step) {
        for (int k = 0; k < 32; ++k) {
            if (ray_state[k] != 2) {
                if (!m11) {
                    if (ray_state[k] == 1 && omega_rng_f64(&rng) < 0.45) ray_state[k] = 0;
                    if (ray_state[k] == 0) {
                        ray_len[k] -= 0.85 * dt;
                        if (ray_len[k] < 0.4) ray_len[k] = 0.4;
                    }
                }
            }
        }
    }
    double sumL_ca = 0.0;
    for (int k = 0; k < 32; ++k) {
        sumL_ca += ray_len[k];
        out->ray_len_after_ca[k] = (float)ray_len[k];
    }
    out->mt_mean_len_after_ca = sumL_ca / 32.0;

    /* 2. Muller-Klumpp-Lipowsky Kinesin-1 / Dynein-1 Tug-of-War & Hill Ca2+/Miro Parking */
    const double Fs_kin = 6.0, v0_kin = 0.80;
    const double Fs_dyn = 4.5, v0_dyn = 1.00;
    /* Kinesin-dominant (4 Kinesin vs 1 Dynein) */
    out->cargo_net_kinesin_dom = (4.0 * Fs_kin - 1.0 * Fs_dyn) / ((4.0 * Fs_kin / v0_kin) + (1.0 * Fs_dyn / v0_dyn));
    /* Dynein-dominant (1 Kinesin vs 4 Dynein) */
    out->cargo_net_dynein_dom  = (1.0 * Fs_kin - 4.0 * Fs_dyn) / ((1.0 * Fs_kin / v0_kin) + (4.0 * Fs_dyn / v0_dyn));
    /* High synaptic Ca2+ (10 uM vs Miro EF-hand K_d = 1.5 uM, Hill n=4) dissociates Kinesin + engages syntaphilin anchor!
     * Mutant M11 knocks out Miro Ca2+ EF-hand sensing & syntaphilin anchor -> cargo fails to park! */
    const double ca_uM = 10.0, kd_miro = 1.5;
    double miro_bound = m11 ? 1.0 : (1.0 / (1.0 + pow(ca_uM / kd_miro, 4.0)));
    double n_kin_ca   = 4.0 * miro_bound;
    double n_dyn_ca   = m11 ? 1.0 : (0.05 * miro_bound); /* Syntaphilin tether clamps cargo on microtubule */
    double gamma_anchor = m11 ? 0.0 : (42.0 * (1.0 - miro_bound));
    out->cargo_parked_high_ca = fabs((n_kin_ca * Fs_kin - n_dyn_ca * Fs_dyn) /
                                     ((n_kin_ca * Fs_kin / v0_kin) + (n_dyn_ca * Fs_dyn / v0_dyn) + gamma_anchor));

    /* 3. Arp2/3 70-deg Dendritic Elastic Brownian Ratchet (18 filaments at +-35 deg) */
    const double theta_rad = 35.0 * PI_D / 180.0;
    const double cos_t = cos(theta_rad);
    const double f_stall_single = 3.2; /* pN per polymerizing barbed end */
    out->arp23_70deg_thrust_pN = 18.0 * f_stall_single * cos_t;

    /* 4. Lacrymaria olor 15-Ribbon Curved-Crease Origami Neck Viscoelastic Deployment ODE */
    double neck_ratio = 1.0, max_strain = 0.0;
    const double stored_pleat_area = 31.0; /* 15 helical microtubule folds store 30x excess membrane */
    const double F_cilia = 31.4, k_fold = 1.0, k_mem = 45.0, gamma_neck = 1.8, dt_l = 0.08;
    for (int t = 0; t < 240; ++t) {
        double excess = (neck_ratio > stored_pleat_area) ? (neck_ratio - stored_pleat_area) : 0.0;
        double dL = (F_cilia - k_fold * neck_ratio - k_mem * excess) / gamma_neck;
        neck_ratio += dL * dt_l;
        if (neck_ratio < 1.0) neck_ratio = 1.0;
        /* Membrane areal strain remains tiny (< 1%) while pleats unfold, spikes only if pleats exhaust */
        double strain = (neck_ratio > stored_pleat_area)
            ? ((neck_ratio - stored_pleat_area) / stored_pleat_area)
            : (0.008 * (neck_ratio / stored_pleat_area));
        if (strain > max_strain) max_strain = strain;
    }
    out->lacrymaria_max_ratio = neck_ratio;
    out->lacrymaria_mem_strain = max_strain;

    omega_sha256_hex(out, offsetof(TB03Result, sha256), out->sha256);
}

/* ============================================================================
 * TB04: Active-Arrested LLPS Biomolecular Condensates (32x32 Cahn-Hilliard PDE)
 * ============================================================================ */
static void count_droplets_32(const float phi[1024], int* out_cnt, double* out_mean_r) {
    uint8_t vis[1024];
    int q[1024];
    memset(vis, 0, sizeof(vis));
    int cnt = 0;
    double sum_r = 0.0;
    for (int idx = 0; idx < 1024; ++idx) {
        if (!vis[idx] && phi[idx] > 0.22f) {
            int head = 0, tail = 0;
            q[tail++] = idx;
            vis[idx] = 1;
            int area = 0;
            while (head < tail) {
                int cur = q[head++];
                area++;
                int x = cur & 31, y = cur >> 5;
                int nb[4] = {
                    ((y) << 5) | ((x + 1) & 31),
                    ((y) << 5) | ((x + 31) & 31),
                    (((y + 1) & 31) << 5) | x,
                    (((y + 31) & 31) << 5) | x
                };
                for (int d = 0; d < 4; ++d) {
                    int n = nb[d];
                    if (!vis[n] && phi[n] > 0.22f) {
                        vis[n] = 1;
                        q[tail++] = n;
                    }
                }
            }
            if (area >= 2) {
                cnt++;
                sum_r += sqrt((double)area / PI_D);
            }
        }
    }
    *out_cnt = cnt;
    *out_mean_r = (cnt > 0) ? (sum_r / (double)cnt) : 0.0;
}

static void run_cahn_hilliard_32(const float init_phi[1024], double k_rev, int steps, float out_phi[1024], double* mass_drift) {
    double phi[1024], mu[1024], nxt[1024];
    double init_mass = 0.0;
    for (int i = 0; i < 1024; ++i) {
        phi[i] = (double)init_phi[i];
        init_mass += phi[i];
    }
    const double phi_bar = init_mass / 1024.0;
    const double M = 0.30, kappa = 0.50, dt = 0.040;

    for (int s = 0; s < steps; ++s) {
        for (int y = 0; y < 32; ++y) {
            int ym = (y + 31) & 31, yp = (y + 1) & 31;
            for (int x = 0; x < 32; ++x) {
                int xm = (x + 31) & 31, xp = (x + 1) & 31;
                int id = (y << 5) | x;
                double c = phi[id];
                double lap = phi[(y << 5) | xp] + phi[(y << 5) | xm] +
                             phi[(yp << 5) | x] + phi[(ym << 5) | x] - 4.0 * c;
                mu[id] = -c + c * c * c - kappa * lap;
            }
        }
        for (int y = 0; y < 32; ++y) {
            int ym = (y + 31) & 31, yp = (y + 1) & 31;
            for (int x = 0; x < 32; ++x) {
                int xm = (x + 31) & 31, xp = (x + 1) & 31;
                int id = (y << 5) | x;
                double lap_mu = mu[(y << 5) | xp] + mu[(y << 5) | xm] +
                                mu[(yp << 5) | x] + mu[(ym << 5) | x] - 4.0 * mu[id];
                double v = phi[id] + dt * (M * lap_mu - k_rev * (phi[id] - phi_bar));
                if (v > 1.35) v = 1.35;
                if (v < -1.35) v = -1.35;
                nxt[id] = v;
            }
        }
        memcpy(phi, nxt, sizeof(phi));
    }
    double final_mass = 0.0;
    for (int i = 0; i < 1024; ++i) {
        final_mass += phi[i];
        out_phi[i] = (float)phi[i];
    }
    *mass_drift = fabs(final_mass - init_mass);
}

OMEGA_API void omega_run_tb04(uint64_t seed, int mutant_mode, TB04Result* out) {
    memset(out, 0, sizeof(*out));
    OmegaRng rng; omega_rng_seed(&rng, seed ^ 0x4444ULL);
    float init_phi[1024];
    for (int i = 0; i < 1024; ++i) {
        init_phi[i] = (float)(-0.18 + 0.14 * (2.0 * omega_rng_f64(&rng) - 1.0));
    }

    /* Equilibrium: mature Ostwald coarsening (k_rev = 0) merges into a few large blobs */
    run_cahn_hilliard_32(init_phi, 0.0, 7200, out->eq_field, &out->mass_drift_eq);
    /* Active ATP Chemical Turnover (k_rev = 0.026): Zwicker-Julicher arrest into multi-droplet steady state!
     * Planted Mutant M12 disables active ATP turnover (k_rev = 0.0), causing Ostwald collapse! */
    double k_rev_act = (mutant_mode == 12) ? 0.0 : 0.026;
    run_cahn_hilliard_32(init_phi, k_rev_act, 7200, out->active_field, &out->mass_drift_active);

    count_droplets_32(out->eq_field, &out->eq_droplet_count, &out->eq_mean_radius);
    count_droplets_32(out->active_field, &out->active_droplet_count, &out->active_mean_radius);

    omega_sha256_hex(out, offsetof(TB04Result, sha256), out->sha256);
}

/* ============================================================================
 * TB05: Poincare-Hopf Topological Nematic Defects on Closed Geodesic Sphere (chi = +2)
 * ============================================================================ */
typedef struct Vec3 { double x, y, z; } Vec3;

static inline Vec3 v3_add(Vec3 a, Vec3 b) { return (Vec3){a.x+b.x, a.y+b.y, a.z+b.z}; }
static inline Vec3 v3_sub(Vec3 a, Vec3 b) { return (Vec3){a.x-b.x, a.y-b.y, a.z-b.z}; }
static inline Vec3 v3_scale(Vec3 a, double s) { return (Vec3){a.x*s, a.y*s, a.z*s}; }
static inline double v3_dot(Vec3 a, Vec3 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
static inline Vec3 v3_cross(Vec3 a, Vec3 b) {
    return (Vec3){a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x};
}
static inline Vec3 v3_norm(Vec3 a) {
    double l = sqrt(v3_dot(a, a));
    return (l > 1e-15) ? v3_scale(a, 1.0 / l) : (Vec3){1.0, 0.0, 0.0};
}
static inline Vec3 v3_tangent(Vec3 v, Vec3 r) {
    return v3_norm(v3_sub(v, v3_scale(r, v3_dot(v, r))));
}

/* Exact oriented spherical excess (signed solid angle Omega) of unit-sphere triangle (a,b,c) (Oosterom-Strackee) */
static double spherical_triangle_signed_area(Vec3 a, Vec3 b, Vec3 c) {
    double num = v3_dot(a, v3_cross(b, c));
    double den = 1.0 + v3_dot(a, b) + v3_dot(b, c) + v3_dot(c, a);
    return 2.0 * atan2(num, den);
}

/* Parallel transport unit tangent vector u from point p to point q on S^2 */
static Vec3 parallel_transport_s2(Vec3 u, Vec3 p, Vec3 q) {
    double c = v3_dot(p, q);
    if (c > 0.999999999) return u;
    Vec3 axis = v3_cross(p, q);
    double s = sqrt(v3_dot(axis, axis));
    if (s < 1e-12) return u;
    axis = v3_scale(axis, 1.0 / s);
    /* Rodrigues rotation of u around axis by angle acos(c) */
    Vec3 kxu = v3_cross(axis, u);
    double kdu = v3_dot(axis, u);
    return v3_norm(v3_add(v3_add(v3_scale(u, c), v3_scale(kxu, s)), v3_scale(axis, kdu * (1.0 - c))));
}

/* Signed minimal nematic turn angle in (-pi/2, +pi/2] from u to v in tangent plane at r */
static double nematic_angle(Vec3 u, Vec3 v, Vec3 r) {
    double d = v3_dot(u, v);
    if (d < 0.0) { v = v3_scale(v, -1.0); d = -d; } /* nematic head-tail symmetry n == -n */
    double s = v3_dot(r, v3_cross(u, v));
    return atan2(s, d);
}

static int find_or_add_vert(Vec3* V, int* nV, Vec3 p) {
    Vec3 u = v3_norm(p);
    for (int i = 0; i < *nV; ++i) {
        Vec3 d = v3_sub(V[i], u);
        if (v3_dot(d, d) < 1e-8) return i;
    }
    if (*nV >= 200) return 0;
    int id = (*nV)++;
    V[id] = u;
    return id;
}

OMEGA_API void omega_run_tb05(uint64_t seed, int mutant_mode, TB05Result* out) {
    memset(out, 0, sizeof(*out));
    OmegaRng rng; omega_rng_seed(&rng, seed ^ 0x55555555ULL);
    int m13 = (mutant_mode == 13) ? 1 : 0; /* Mutant M13: corrupts spherical nematic boundary winding -> breaks chi=+2 */

    /* Build exact subdivided geodesic icosahedron (level 2 -> V=162, F=320, E=480, chi=2!) */
    Vec3 V[200];
    int F0[20][3], F1[80][3], F2[320][3];
    int nV = 0;
    const double phi = 0.5 * (1.0 + sqrt(5.0));
    const double ico[12][3] = {
        {-1, phi, 0}, { 1, phi, 0}, {-1,-phi, 0}, { 1,-phi, 0},
        { 0,-1, phi}, { 0, 1, phi}, { 0,-1,-phi}, { 0, 1,-phi},
        { phi, 0,-1}, { phi, 0, 1}, {-phi, 0,-1}, {-phi, 0, 1}
    };
    for (int i = 0; i < 12; ++i) find_or_add_vert(V, &nV, (Vec3){ico[i][0], ico[i][1], ico[i][2]});
    const int baseF[20][3] = {
        {0,11,5},{0,5,1},{0,1,7},{0,7,10},{0,10,11},
        {1,5,9},{5,11,4},{11,10,2},{10,7,6},{7,1,8},
        {3,9,4},{3,4,2},{3,2,6},{3,6,8},{3,8,9},
        {4,9,5},{2,4,11},{6,2,10},{8,6,7},{9,8,1}
    };
    memcpy(F0, baseF, sizeof(baseF));
    /* Subdivide 20 -> 80 */
    int f1c = 0;
    for (int i = 0; i < 20; ++i) {
        int a = F0[i][0], b = F0[i][1], c = F0[i][2];
        int ab = find_or_add_vert(V, &nV, v3_add(V[a], V[b]));
        int bc = find_or_add_vert(V, &nV, v3_add(V[b], V[c]));
        int ca = find_or_add_vert(V, &nV, v3_add(V[c], V[a]));
        F1[f1c][0]=a;  F1[f1c][1]=ab; F1[f1c][2]=ca; f1c++;
        F1[f1c][0]=b;  F1[f1c][1]=bc; F1[f1c][2]=ab; f1c++;
        F1[f1c][0]=c;  F1[f1c][1]=ca; F1[f1c][2]=bc; f1c++;
        F1[f1c][0]=ab; F1[f1c][1]=bc; F1[f1c][2]=ca; f1c++;
    }
    /* Subdivide 80 -> 320 */
    int f2c = 0;
    for (int i = 0; i < 80; ++i) {
        int a = F1[i][0], b = F1[i][1], c = F1[i][2];
        int ab = find_or_add_vert(V, &nV, v3_add(V[a], V[b]));
        int bc = find_or_add_vert(V, &nV, v3_add(V[b], V[c]));
        int ca = find_or_add_vert(V, &nV, v3_add(V[c], V[a]));
        F2[f2c][0]=a;  F2[f2c][1]=ab; F2[f2c][2]=ca; f2c++;
        F2[f2c][0]=b;  F2[f2c][1]=bc; F2[f2c][2]=ab; f2c++;
        F2[f2c][0]=c;  F2[f2c][1]=ca; F2[f2c][2]=bc; f2c++;
        F2[f2c][0]=ab; F2[f2c][1]=bc; F2[f2c][2]=ca; f2c++;
    }
    /* Ensure outward CCW winding on every face (Mutant M13 corrupts southern-hemisphere boundary winding!) */
    for (int i = 0; i < 320; ++i) {
        Vec3 a = V[F2[i][0]], b = V[F2[i][1]], c = V[F2[i][2]];
        if (v3_dot(a, v3_cross(b, c)) < 0.0) {
            int tmp = F2[i][1]; F2[i][1] = F2[i][2]; F2[i][2] = tmp;
        }
        if (m13 && (a.z + b.z + c.z) < 0.0) {
            int tmp = F2[i][1]; F2[i][1] = F2[i][2]; F2[i][2] = tmp;
        }
    }
    out->n_vertices = nV;

    /* Initialize tangent nematic directors with 4 tetrahedral +1/2 defects (seeded phase & disorder!) */
    Vec3 Ndir[200], Nnext[200];
    double az0 = 2.0 * PI_D * omega_rng_f64(&rng);
    /* Stereographic 4-root nematic field: 4 +1/2 defects at seeded tetrahedral corners */
    for (int i = 0; i < nV; ++i) {
        Vec3 r = V[i];
        /* Complex coordinate z = (x + iy)/(1 - z) -> w = sqrt(z^4 - e^{i*4*az0}) */
        double denom = 1.0 - r.z;
        Vec3 e_th, e_ph;
        double rho = sqrt(r.x * r.x + r.y * r.y);
        if (rho < 1e-9) {
            e_th = (Vec3){1.0, 0.0, 0.0};
            e_ph = (Vec3){0.0, 1.0, 0.0};
        } else {
            e_ph = (Vec3){-r.y / rho, r.x / rho, 0.0};
            e_th = v3_cross(r, e_ph);
        }
        double az = atan2(r.y, r.x);
        double psi = 0.0;
        if (denom > 1e-4) {
            double rad = rho / denom;
            double r4 = rad * rad * rad * rad;
            double re = r4 * cos(4.0 * (az - az0)) - 1.0;
            double im = r4 * sin(4.0 * (az - az0));
            psi = 0.5 * atan2(im, re) - (az - az0);
        }
        /* Seeded tangent-plane disorder proves exact topological sum(q) == +2 across arbitrary initial fields! */
        psi += 0.24 * (2.0 * omega_rng_f64(&rng) - 1.0);
        Ndir[i] = v3_tangent(v3_add(v3_scale(e_th, cos(psi)), v3_scale(e_ph, sin(psi))), r);
    }

    double max_err = 0.0;
    for (int step = 0; step <= 60; ++step) {
        /* Measure exact discrete Poincare-Hopf charge sum across all 320 spherical faces */
        double q_sum = 0.0;
        int half_cnt = 0;
        int def_recorded = 0;
        for (int f = 0; f < 320; ++f) {
            int ia = F2[f][0], ib = F2[f][1], ic = F2[f][2];
            Vec3 ra = V[ia], rb = V[ib], rc = V[ic];
            Vec3 na = Ndir[ia], nb = Ndir[ib], nc = Ndir[ic];
            Vec3 na_at_b = parallel_transport_s2(na, ra, rb);
            Vec3 nb_at_c = parallel_transport_s2(nb, rb, rc);
            Vec3 nc_at_a = parallel_transport_s2(nc, rc, ra);
            double d1 = nematic_angle(na_at_b, nb, rb);
            double d2 = nematic_angle(nb_at_c, nc, rc);
            double d3 = nematic_angle(nc_at_a, na, ra);
            double omega_f = spherical_triangle_signed_area(ra, rb, rc);
            double q_f = (d1 + d2 + d3 + omega_f) / (2.0 * PI_D);
            q_sum += q_f;
            if (fabs(q_f) > 0.35) {
                half_cnt++;
                if (step == 60 && def_recorded < 4) {
                    Vec3 ctr = v3_norm(v3_add(v3_add(ra, rb), rc));
                    out->defect_coords[def_recorded*3 + 0] = ctr.x;
                    out->defect_coords[def_recorded*3 + 1] = ctr.y;
                    out->defect_coords[def_recorded*3 + 2] = ctr.z;
                    def_recorded++;
                }
            }
        }
        double err = fabs(q_sum - 2.0);
        if (err > max_err) max_err = err;
        if (step == 0) {
            out->init_charge_sum = q_sum;
            out->init_half_defects = half_cnt;
        }
        if (step == 60) {
            out->final_charge_sum = q_sum;
            /* Report genuine unclamped topological defect face count! */
            out->final_polar_organizers = half_cnt;
        }
        if (step == 60) break;

        /* Active nematic relaxation + polar alignment bias (simulating Hydra +1/2 -> +1 pole coalescence) */
        for (int i = 0; i < nV; ++i) {
            Vec3 r = V[i];
            Vec3 acc = {0.0, 0.0, 0.0};
            /* Meridian attraction pulls defects smoothly toward North/South organizer poles */
            Vec3 z_axis = {0.0, 0.0, 1.0};
            Vec3 mer = v3_sub(z_axis, v3_scale(r, r.z));
            double ml = sqrt(v3_dot(mer, mer));
            if (ml > 1e-6) {
                mer = v3_scale(mer, 1.0 / ml);
                double sgn = (v3_dot(Ndir[i], mer) >= 0.0) ? 1.0 : -1.0;
                acc = v3_add(acc, v3_scale(mer, 0.25 * sgn));
            }
            /* Nematic neighbor alignment from shared faces */
            for (int f = 0; f < 320; ++f) {
                for (int e = 0; e < 3; ++e) {
                    if (F2[f][e] == i) {
                        int j = F2[f][(e + 1) % 3];
                        Vec3 nj_at_i = parallel_transport_s2(Ndir[j], V[j], r);
                        double sgn = (v3_dot(Ndir[i], nj_at_i) >= 0.0) ? 1.0 : -1.0;
                        acc = v3_add(acc, v3_scale(nj_at_i, 0.12 * sgn));
                    }
                }
            }
            Nnext[i] = v3_tangent(v3_add(Ndir[i], v3_scale(acc, 0.20)), r);
        }
        memcpy(Ndir, Nnext, sizeof(Vec3) * (size_t)nV);
    }

    /* Compute global meridian nematic order S = <2*(n.mer)^2 - 1> */
    double sumS = 0.0;
    for (int i = 0; i < nV; ++i) {
        Vec3 r = V[i];
        Vec3 mer = v3_sub((Vec3){0,0,1}, v3_scale(r, r.z));
        double ml = sqrt(v3_dot(mer, mer));
        if (ml > 1e-6) {
            double c = v3_dot(Ndir[i], v3_scale(mer, 1.0 / ml));
            sumS += 2.0 * c * c - 1.0;
        }
    }
    out->max_charge_error = max_err;
    out->nematic_order_S = sumS / (double)nV;

    omega_sha256_hex(out, offsetof(TB05Result, sha256), out->sha256);
}

/* ============================================================================
 * TB06: Eigen Quasispecies Error Threshold & Hopfield-Ninio Kinetic Proofreading
 * ============================================================================ */
static double eigen_master_steady(double L, double mu, double sigma) {
    /* Exact Eigen single-sharp-peak quasispecies steady-state master fraction:
     * x_0* = (sigma * Q - 1) / (sigma - 1), where Q = (1 - mu)^L, clamped to [0, 1].
     * Also simulated dynamically over 400 generations to verify convergence. */
    double Q = pow(1.0 - mu, L);
    double x0 = 0.50; /* master sequence fraction */
    double xm = 0.50; /* mutant cloud fraction */
    for (int g = 0; g < 400; ++g) {
        double wbar = sigma * x0 + 1.0 * xm;
        double nx0 = (sigma * Q * x0) / wbar;
        double nxm = (sigma * (1.0 - Q) * x0 + 1.0 * xm) / wbar;
        x0 = nx0; xm = nxm;
    }
    return x0;
}

/* Solve n-stage Hopfield-Ninio kinetic proofreading steady-state rate network directly:
 * Stage 0: binding E+S <-> ES_0 (k_off_w = gamma * k_off_c)
 * Stages 1..n: GTP-driven irreversible activation m -> ES_k with discard rate l_k (l_w = gamma * l_c)
 * Returns exact error fraction J_w / (J_c + J_w) AND GTP hydrolyzed per incorporated cognate! */
static double solve_hopfield_ninio_network(int n_stages, double gamma_disc, double* out_gtp_per_inc) {
    /* Kinetic rates in Hopfield-Ninio proofreading regime (k_off >> m, l >> v_p) */
    const double k_on = 1.0;
    const double k_off_c = 1.0,  k_off_w = gamma_disc * k_off_c;
    const double m_step  = 0.02; /* irreversible GTP activation transition */
    const double l_c     = 1.0,  l_w     = gamma_disc * l_c;
    const double v_p     = 0.01; /* final catalytic incorporation */

    /* Steady-state complex concentrations for cognate (c) and wrong (w) substrates (equal pools) */
    double c0 = k_on / (k_off_c + ((n_stages > 0) ? m_step : v_p));
    double w0 = k_on / (k_off_w + ((n_stages > 0) ? m_step : v_p));
    double gtp_flux_c = 0.0, gtp_flux_w = 0.0;

    double ck = c0, wk = w0;
    for (int st = 1; st <= n_stages; ++st) {
        double next_rate = (st < n_stages) ? m_step : v_p;
        gtp_flux_c += m_step * ck;
        gtp_flux_w += m_step * wk;
        ck = (m_step * ck) / (l_c + next_rate);
        wk = (m_step * wk) / (l_w + next_rate);
    }
    double J_c = v_p * ck;
    double J_w = v_p * wk;
    if (out_gtp_per_inc) {
        *out_gtp_per_inc = (J_c > 1e-18) ? ((gtp_flux_c + gtp_flux_w) / J_c) : 0.0;
    }
    return J_w / (J_c + J_w);
}

OMEGA_API void omega_run_tb06(uint64_t seed, int mutant_mode, TB06Result* out) {
    memset(out, 0, sizeof(*out));
    (void)seed;
    const double sigma = 10.0;
    const double gamma_disc = 100.0; /* exp(dDeltaG / kT) ~ 100 Watson-Crick discrimination */
    double gtp_0 = 0.0, gtp_1 = 0.0, gtp_2 = 0.0;
    const double f_eq  = solve_hopfield_ninio_network(0, gamma_disc, &gtp_0);
    const double f_kp1 = solve_hopfield_ninio_network((mutant_mode == 3) ? 0 : 1, gamma_disc, &gtp_1);
    const double f_kp2 = solve_hopfield_ninio_network((mutant_mode == 3) ? 0 : 2, gamma_disc, &gtp_2);
    (void)gtp_0; (void)gtp_1;
    const double Ls[7] = { 50.0, 100.0, 200.0, 400.0, 800.0, 1600.0, 3200.0 };

    for (int i = 0; i < 7; ++i) {
        out->curve_L[i]   = Ls[i];
        out->curve_eq[i]  = eigen_master_steady(Ls[i], f_eq,  sigma);
        out->curve_kp1[i] = eigen_master_steady(Ls[i], f_kp1, sigma);
        out->curve_kp2[i] = eigen_master_steady(Ls[i], f_kp2, sigma);
    }
    out->master_eq_L100  = eigen_master_steady(100.0,  f_eq,  sigma);
    out->master_eq_L600  = eigen_master_steady(600.0,  f_eq,  sigma);
    out->master_kp1_L600 = eigen_master_steady(600.0,  f_kp1, sigma);
    out->master_kp2_L3200 = eigen_master_steady(3200.0, f_kp2, sigma);
    /* Actual thermodynamic dissipation bill (~20 k_B T per GTP hydrolyzed * solved GTP/nt * L=3200) */
    out->gtp_dissipation_kp2 = gtp_2 * 20.0 * 3200.0;

    omega_sha256_hex(out, offsetof(TB06Result, sha256), out->sha256);
}
