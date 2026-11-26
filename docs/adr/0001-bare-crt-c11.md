# ADR 0001: Zero-Dependency C11 Core with `-ffp-contract=off` and Compiled Planted Mutants

- **Status:** Accepted (`v1.0.0`, tightened in `v5.2.0` / `v5.3.0`)
- **Date:** `2026-11-18` (updated `2026-11-26`)

## Context
Multi-scale cellular biophysics simulations frequently suffer from heavy transitive library bloat, compiler-dependent floating-point contraction (FMA) drift between Windows Clang and Linux GCC, and unverified test suites that could pass tautologically even when physics mechanisms are broken.

## Decision
1. Implement all 16 biophysics testbeds (`TB01`–`TB16`), Clean-Slate v3.0 75 substrates + 1,000-cell Hybrid Cube, counter-based `Philox4x32-10` PRNG (Salmon et al., SC11), and FIPS 180-4 SHA-256 in pure zero-dependency C11 (`-O3 -std=c11 -Wall -Wextra -Wshadow -Werror`).
2. Enforce `-ffp-contract=off` identically in both `build.ps1` (Win64 Clang) and `build_linux.sh` (Linux GCC/Clang).
3. Embed `16` runtime-selectable compiled structural knockouts (`--mutant 1..16`) and verify that `test_omega_falsification.c` (`60` gates) catches `16/16` mutants.

## Consequences
- **Positive:** Complete engine runs in `~141 ms` (`4.85 ms` for 75 substrates + 1,000 cube), builds in `< 5 s` with zero external package managers, and prevents tautological green tests.
- **Negative:** Disabling FMA trades minor theoretical throughput for cross-OS numerical consistency; transcendental `libm` differences across non-x86 architectures remain a documented limitation (`LIMITATIONS.md`).
