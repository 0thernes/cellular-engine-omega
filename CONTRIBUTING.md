# Contributing to CELLULAR ENGINE OMEGA

Thank you for your interest in auditing, replicating, or extending CELLULAR ENGINE OMEGA.

## Scientific & Engineering Standards

1. **Zero Fabrication & Honest Boundaries:** Every claim, benchmark, SHA-256 digest, and theorem count must come from a real execution of the current commit. Never fabricate numbers, citations, or kernel runs.
2. **C11 Clean-Room Discipline:** Code under `src/`, `include/`, and `test/` must compile cleanly with zero warnings under `-O3 -std=c11 -Wall -Wextra -Wshadow -Werror -ffp-contract=off` on both Win64 Clang and Linux GCC, pass all 60 falsification gates (`16/16` mutants), and pass the fuzz/property harness (`test/fuzz_omega_cli_codec.c`).
3. **Pure Lean 4.35.0 Discipline:** Formalizations under `lean/` must remain zero-Mathlib, use `module` headers with `autoImplicit = false` and `relaxedAutoImplicit = false`, maintain isolated `Spec.lean` -> `Challenge.lean` / `Solution.lean` parity, depend strictly on standard Lean axioms (`propext`, `Quot.sound`, `Classical.choice` max; zero `sorryAx`, zero `native_decide`, zero compiler/SAT axioms), and pass multi-kernel verification.
4. **Repository Hygiene:** Keep tracked files under 5 MB, commit no secrets or `.env` files, and update `CLAIMS.md`, `CHANGELOG.md`, `docs/statement-fidelity.md`, and `CHECKSUMS.sha256` whenever behavior or proofs change.
5. **Finding Classification:** Classify all audit findings into one of the 10 Build Standard v1.2.0 classes (`bug`, `error`, `issue`, `problem`, `concern`, `dilemma`, `conflict`, `gap`, `hole`, `blindspot`) using the labels in `.github/labels.tsv`.

## Licensing

Contributions to code, Lean proofs, and scripts are accepted under **Apache-2.0** ([LICENSE](LICENSE)); contributions to prose documentation, white papers, and figures are accepted under **CC-BY-4.0** ([LICENSE-DOCS](LICENSE-DOCS)).
