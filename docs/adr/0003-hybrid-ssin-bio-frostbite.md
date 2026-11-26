# ADR 0003: Bio-Frostbite Lafont Interaction Nets & Live C11-to-Lean Telemetry Receipts

- **Status:** Accepted (`v4.0.0`, expanded `v5.1.0`–`v5.2.0`)
- **Date:** `2026-11-21`

## Context
Traditional shared-memory cellular simulations exhibit lock contention, scheduling-order nondeterminism, and disconnected formal proofs that verify toy constants unrelated to the actual compiled C11 binary.

## Decision
1. Model 4-layer biological execution (`TB14`) as a single-ownership Lafont Interaction Net VM (`src/exp_14_bio_inet.c`) verifying 0-bit SHA-256 strong confluence across FIFO, LIFO, and chaotic Random schedulers plus a Hybrid SSIN arbiter (`>= 95%` lock-free rewrites) and zero-power cryptobiotic port freezing.
2. Add `omega_cell --export-lean-trace` (`src/omega_main_cli.c`) to auto-emit live C11 telemetry and SHA-256 digests directly into `lean/OmegaCellularProofs/LiveC11Receipt.lean`, verified inside the Lean 4 kernel by `O63` and `O72`.

## Consequences
- **Positive:** Eliminates drift between compiled C11 runs and Lean proof artifacts while enforcing exact integer conservation across the 6-tier stack (`TB14` -> `TB16`).
- **Negative:** Whenever a C11 substrate/cube kernel intentionally changes (e.g., `PM21` 16-trial fix), `LiveC11Receipt.lean` must be regenerated before `lake build` (automated in `build.ps1` and `build_linux.sh`).
