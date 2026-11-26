# Benchmarks & Performance Budgets (`A37` / `B26` / `B27`)

All measurements below are recorded from **10 warmed runs** (after 1 warmup run) by `scripts/run_stats_and_benchmarks.py` and appended to [`results/bench-history.csv`](../results/bench-history.csv).

## 1. Latency Budgets vs. Measured Performance (`v5.2.0`, Windows 11 x86_64, Clang `-O3 -flto=thin -ffp-contract=off`)

| Workload | Budget Target | Measured Median | Measured p95 | Status |
| :--- | :--- | ---: | ---: | :--- |
| Full 16-Testbed Suite (`omega_cell all --seed 2026001`) | `< 500 ms` | **`113.18 ms`** | **`128.34 ms`** | **PASS** (4.4× headroom) |
| Clean-Slate v3.0 75 Substrates + 1,000-Cell Hybrid Cube (`omega_cell cube`) | `< 100 ms` | **`28.25 ms`** | **`48.92 ms`** | **PASS** (3.5× headroom) |
| Full 60-Gate / 16-Mutant / 10-Seed C11 Falsification Suite | `< 3,000 ms` | **`1,030.43 ms`** | **`1,090.27 ms`** | **PASS** (2.9× headroom) |
| Python Verification Ladder (`35` tests, `1,200+` Hypothesis cases) | `< 15,000 ms` | **`3,270 ms`** | — | **PASS** (4.6× headroom) |
| Optimization Parity (`-O3` vs `-O0`, single run, Seed `2026001`) | `0-bit SHA drift` | `90.72 ms` (`-O3`) vs `340.67 ms` (`-O0`) | `3.76×` speedup | **PASS** (Bit-exact SHA-256 `a9264184...`) |

## 2. Artifact & Memory Budgets

| Resource | Hard Budget | Measured (`v5.2.0`) | Enforcement |
| :--- | :--- | :--- | :--- |
| Coherent Quantum Statevector Memory (`omega_substrates_cube.c`) | `Q <= 10 -> 16 KiB` | `16,384 B` | Kernel-proved (`O70`, `O72`) + C11 gate `G55` |
| Balanced Ternary Packing (`PM03` `b1.58`) | `3^5 <= 256` states/byte | `243 <= 256` | Kernel-proved (`O71`, `O72`) + C11 gate `G55` |
| Social Preview Card (`docs/media/social-preview.png`) | `1280×640`, `< 1 MB` | `1280×640`, `85.4 KB` | `scripts/generate_figures.py` |
| Tracked Repository File Size Ceiling (`A24`) | `< 5 MB` per tracked file | All `< 1 MB` | `scripts/Audit-Repo.ps1` |
| Web Observatory Concurrent Subprocesses (`web/server.mjs`) | `MAX_INFLIGHT = 2` | `429` beyond 2 | Live HTTP test in CI |

## 3. Reproducing Benchmarks

```bash
python3 scripts/run_stats_and_benchmarks.py
```
History is logged append-only in [`results/bench-history.csv`](../results/bench-history.csv).
