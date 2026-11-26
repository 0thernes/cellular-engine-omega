# System Architecture (`A20`)

**CELLULAR ENGINE OMEGA (`v5.3.0`)** integrates a zero-dependency C11 multi-scale biophysics simulation core, a Clean-Slate v3.0 75-substrate & 1,000-cell Hybrid Cube verifier, a Lean 4.35.0 suite of 100 discrete lemmas about the software model, linked to the C11 output by a generated receipt file whose integer and string fields `O63` and `O72` re-check on each build (checked by the reference kernel and four independent checkers), a Python exact verification ladder, FAIR scholarly metadata, and a hardened loopback Web Observatory.

```mermaid
flowchart TD
    subgraph C11["Native C11 Zero-Dependency Engine (-O3 -std=c11 -Werror -ffp-contract=off)"]
        RNG["omega_rng_hash.c\nPhilox4x32-10 PRNG + FIPS 180-4 SHA-256"]
        TB1["exp_01_06.c + exp_07_12.c\nTB01..TB12 Core Biophysics"]
        TB2["exp_13..16_*.c\nTB13 12D Morphospace · TB14 Bio-Frostbite INet\nTB15 Deep Strata · TB16 Virtual-Cell AI"]
        CUBE["omega_substrates_cube.c\n75 Substrates (PM/MF/GW) + 1,000 Hybrid Cube"]
        CLI["omega_main_cli.c\nCLI · JSON API · --export-lean-trace"]
        RNG --> TB1 & TB2 & CUBE
        TB1 & TB2 & CUBE --> CLI
    end

    subgraph VERIFY_C["C11 Falsification & Fuzzing"]
        FALS["test_omega_falsification.c\n60 Gates (G01..G60) · 16/16 Mutants · 10 Seeds"]
        FUZZ["fuzz_omega_cli_codec.c\nlibFuzzer + 256 Mutated Cases + 32 prop_check"]
        TB1 & TB2 & CUBE --> FALS & FUZZ
    end

    subgraph LEAN["Pure Lean 4.35.0 Formal Suite (Zero Mathlib, Constructive)"]
        RECEIPT["LiveC11Receipt.lean\nAuto-emitted C11 Telemetry & SHAs"]
        SPEC["Spec.lean\nPure Definitions Only (0 Proofs)"]
        CHAL["Challenge.lean\nIsolated Statement-Only Specs (100)"]
        SOL["Solution.lean + 14 Proof Modules\n100 Constructive Proofs (O01..O100)"]
        KERN["Reference kernel + 4 independent checkers + negative elaboration test\nlean · leanchecker-paranoid · lean4lean\nnanoda_bin · con-leche · con-ron · comparator"]
        CLI -- "--export-lean-trace" --> RECEIPT
        RECEIPT --> SPEC
        SPEC --> CHAL & SOL
        CHAL & SOL --> KERN
    end

    subgraph PY["Python Exact Verification & Paper Pipeline"]
        ORACLE["reference/biophysics_oracle.py\nExact Fraction / 50-digit Decimal Oracle"]
        PYTEST["tests/test_omega_verification_ladder.py\nHypothesis 1,200+ Cases · Schemas · Differential"]
        STATS["scripts/run_stats_and_benchmarks.py\n20-Seed Permutation + Bootstrap + Holm-Bonferroni"]
        PAPER["paper/main.tex -> dist/Donahue_2026_cellular_engine_omega.pdf\nAMS-LaTeX White Paper + Vector PDFs + Blueprint"]
        CLI --> PYTEST & STATS
        ORACLE --> PYTEST
        STATS --> PAPER
    end

    subgraph WEB["Hardened Loopback Web Observatory (127.0.0.1)"]
        SRV["web/server.mjs\nCSP · Host/Origin Guard · /api/health /healthz /api/run /api/cube"]
        UI["web/index.html + app.js + styles.css\nWCAG 2.2 AA Interactive Explorer"]
        CLI --> SRV --> UI
    end
```

## 6-Tier Coupled Conservation Stack (`O40`, `O58`, `TB14` -> `TB16`)

1. **Tier 1 (DNA / Molecular Code):** Proofreading (`TB06`), CISS chirality & *Oxytricha* pointer-chain unscrambling (`TB15`), linear `polyDup` active pairs (`TB14`).
2. **Tier 2 (Organelles & Active Matter):** Flagellar `MotA5MotB2` & `F1F0` gearboxes (`TB01`), microtubule Euler buckling & motors (`TB03`), active LLPS Ostwald arrest (`TB04`), Cohesin TADs (`TB15`).
3. **Tier 3 (Cellular Dynamics & Topology):** Spherical `S^2` Poincaré–Hopf nematic defects (`TB05`), Turing/Feinberg morphogenesis (`TB07`), Gillespie TUR & chemotaxis (`TB08`), Levin `Vmem` bistability (`TB15`).
4. **Tier 4 (Multicellular Organisms `<= 1,000` Cells):** 4-Hox 7-organ eutelic tardigrade (`TB09`), odd-elastic active solids (`TB10`), endosymbiotic bioenergetics (`TB11`), *Rhombozoa*/*Henneguya* floors (`TB15`).
5. **Tier 5 (Ecosystems & Open Proliferation):** Virosphere Kill-the-Winner & Mavirus rescue (`TB02`), somatic governance Track A/B (`TB12`), 12-axis open morphospace (`TB13`).
6. **Tier 6 (Physics-Projected Virtual-Cell AI & Hybrid Cube):** `O56` exact hyperplane conservation projection + JCVI-syn3A manifold (`TB16`) + 75 computational substrates & `10×10×10` 1,000-cell Hybrid Cube (`omega_substrates_cube.c`).
