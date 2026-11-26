# HANDOFF.md — cellular-engine-omega (`v5.2.0`)

**Status:** Verified green on Win64 Clang (`-Wall -Wextra -Wshadow -Werror -ffp-contract=off`), Linux GCC + ASan/UBSan (`-Wall -Wextra -Wshadow -Werror -ffp-contract=off`), Python verification ladder (`35/35` pytest + Hypothesis `1,200+` cases), and multi-kernel Pure Lean 4.35.0 (`89/89` constructive proofs, `0` `Classical.choice`, `0` Mathlib).

- **Author:** Alexander Stephen Donahue (ORCID `0009-0006-8168-9648`, Independent Researcher, Spokane, WA, USA)
- **Single Version Source:** `VERSION` (`5.2.0`)
- **Dual License:** Apache-2.0 (`LICENSE`) + CC BY 4.0 (`LICENSE-DOCS`)
- **Verified C11 Scope:** `60/60` falsification gates, `16/16` planted mutants, `256/256` fuzz cases, `75/75` Clean-Slate v3.0 substrates (`20/20` seeds), `1,000/1,000` Hybrid Cube (`661 [OK] / 274 [ADAPT] / 20 [OVER] / 45 [EXCL]`, `20/20` seeds).
- **Verified Lean 4.35.0 Scope:** `89/89` theorems (`O01`–`O89`), strictly constructive within `{propext, Quot.sound}`, isolated `Spec.lean -> Challenge.lean / Solution.lean`, audited across `lean`, `leanchecker`, `leanchecker-paranoid`, `con-leche --verified`, `con-ron --verified`, and Linux CI `lake comparator --config comparator.json --paranoid`.
- **White Paper & Artifacts:** `paper/main.tex` -> `dist/Donahue_2026_cellular_engine_omega.pdf` + `dist/arxiv_submission.tar.gz`, `blueprint/index.html`, `pages/index.html`.
- **Honest Mega-Atlas Boundary:** Solves **`0 / 1,900`** overarching open problems in `0thernes/unsolved-problems-mega-atlas` (`docs/mega-atlas-cross-check.md`).
- **PowerShell Rule:** Always pass `-LiteralPath` on `Z:\[Vibe Coded (AI)]\...` paths.
