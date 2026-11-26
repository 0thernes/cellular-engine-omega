# Preregistration of Quantitative Hypotheses & Falsification Gates (`B32`)

**Project:** CELLULAR ENGINE OMEGA (`v5.2.0`)  
**Author:** Alexander Stephen Donahue (ORCID `0009-0006-8168-9648`, Independent Researcher, Spokane, WA, USA)  
**Date Locked:** `2026-11-25` (prior to executing multi-seed statistical sweep `scripts/run_stats_and_benchmarks.py` on held-out seeds `2026001`–`2026020`)

---

## 1. Primary Confirmatory Hypotheses (Pre-Specified Effect Directions & Statistical Controls)

All hypotheses below are evaluated across **`N = 20` independent deterministic seeds (`2026001` through `2026020`)** comparing intact physical mechanisms (**Treatment**) against compiled structural knockout / ablation mutants (**Control**) using:
- **Test:** Two-sided exact paired permutation test (`2^N` or `10,000`-permutation seeded Monte Carlo with fixed seed `20261125`),
- **Effect Size:** Paired Cohen's $d_z$ and median paired ratio/difference,
- **Uncertainty:** `10,000`-resample seeded bootstrap `95%` percentile confidence intervals,
- **Multiplicity Control (`B31`):** Holm–Bonferroni family-wise error rate control across all `K = 8` confirmatory contrasts at family-wise $\alpha = 0.05$.

| ID | Domain | Treatment (Intact) | Matched Control (Knockout) | Primary Endpoint | Pre-Specified Direction & Threshold |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `H1` | `TB02` Virosphere | Intact Mavirus virophage + Giant Virus | `M2` (Mavirus disabled) | Amoeba host survival fraction | Intact $>$ Control; Holm-adjusted $p < 0.01$; survival ratio $> 2\times$ |
| `H2` | `TB03` Cytoskeleton | Intact `Ca2+` MT catastrophe & Miro parking | `M11` (`Ca2+` catastrophe/parking disabled) | Post-`Ca2+` mean microtubule length (`mt_after_ca`) | Intact $<$ Control (`>50%` melt); Holm-adjusted $p < 0.01$ |
| `H3` | `TB04` Active LLPS | Active ATP turnover (`k_rev > 0`) | `M12` (Equilibrium passive Ostwald) | Arrested droplet count (`active_droplets`) | Intact $>$ Control; Holm-adjusted $p < 0.01$ |
| `H4` | `TB06` Proofreading | Hopfield–Ninio KP1 (`L = 600`) | `M3` (Proofreading disabled) | Master sequence survival fraction | Intact $>$ Control; Holm-adjusted $p < 0.01$ |
| `H5` | `TB07` Morphogenesis | Fast inhibitor (`D_v = 30 D_u`) | `M4` (Equal diffusion `D_u = D_v`) | Spatial Turing pattern std | Intact $>$ Control; Holm-adjusted $p < 0.01$; Intact $> 0.05$, Control $< 10^{-3}$ |
| `H6` | `TB09` Tardigrade | 3-tier CAHS/Dsup/DODA1/TRID1 | `M5` (Stress-shielding KO) | Extreme radiation+desiccation survival | Intact $>$ Control; Holm-adjusted $p < 0.01$; ratio $> 50\times$ |
| `H7` | `TB14` Bio-Frostbite | Linear single-ownership INet | `M8` (Shared-RAM order race) | FIFO/LIFO/Random confluence match & integer mass drift | Intact: `100%` confluence & `0` drift; Control: `0%` confluence & drift $> 0$ |
| `H8` | `TB16` Virtual-Cell AI | `O56` hyperplane physics projection | `M10` (Physics projection disabled) | Projected conservation residual (`proj_mass_res`) | Intact $< 10^{-12}$ vs Control $> 0.15$; Holm-adjusted $p < 0.01$ |

## 2. Secondary Confirmatory Stability & Held-Out ML Endpoints

- **Multi-Seed Gate Stability (`S1`):** `60 / 60` C11 falsification gates and `75 / 75` substrates (`PM01`–`PM25`, `MF01`–`MF25`, `GW01`–`GW25`) + `1,000 / 1,000` Hybrid Cube partitions must pass on `20 / 20` seeds (`100%`, zero flakes).
- **Held-Out Zero-Shot Perturbation Evaluation (`S2`, `B33`):** `TB16` Ridge perturbation model trained strictly on training split must maintain held-out test $R^2 \ge 0.85$ and held-out AUROC $\ge 0.95$ across all `20` seeds with zero train/test index overlap (verified by source & runtime audit).

## 3. Stopping & Reporting Rules

- No seeds are excluded, cherry-picked, or replaced after execution.
- All `20` seed rows are archived raw in `results/multiseed_stats.csv`.
- Any deviation from this preregistration must be logged explicitly in `DEVIATIONS.md`.
