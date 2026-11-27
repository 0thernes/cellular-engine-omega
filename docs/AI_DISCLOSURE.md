# AI-Assistance Disclosure & Scholarly Accountability Statement

**Author:** Alexander Stephen Donahue (ORCID `0009-0006-8168-9648`, Independent Researcher, Spokane, WA, USA)  
**Repository:** `https://github.com/0thernes/cellular-engine-omega` (`v5.4.0`)  
**Policy Alignment:** COPE, ICMJE, arXiv, Zenodo, and *Defensible Independent AI-Assisted Research Playbook (v1.3 §4)*

---

## 1. Transparent Disclosure of AI Roles

Large language model (LLM) coding and research assistants (Google DeepMind Antigravity / Gemini and Anthropic Claude workflows) were used as **interactive engineering, formalization, drafting, and adversarial auditing tools** across the construction and multi-cycle hardening of CELLULAR ENGINE OMEGA (`v1.0.0` through `v5.4.0`), specifically to:

- Draft and refactor C11 simulation kernels, falsification gates (`G01`–`G60`), fuzz harnesses, Python verification oracles, and HTML/CSS/JS Web Observatory components,
- Explore tactic proofs, constructive refactors (`0` `Classical.choice`), and formal-statement cleanups (`O01`, `O56`) in Pure Lean 4.35.0 (`O01`–`O100`),
- Assist in structuring AMS-LaTeX prose, BibTeX metadata, documentation templates, FAIR JSON-LD descriptors, and Build Standard v1.2.0 compliance checks,
- Execute adversarial red-team and statement-audit passes identifying real defects (including the `v5.1.0` `PM21` multi-seed stochastic rounding variance bug, Linux `-ffp-contract=off` FMA divergence, PRNG documentation mislabeling, the `v5.3.0` Nüchter 2006 DOI typo `...079` -> `...089`, unused `O01` rate hypotheses, and the tautological `O56` second conjunct).

## 2. Human Direction, CRediT Attribution & Sole Accountability

- **No AI system is listed as an author.** Alexander Stephen Donahue is the sole author (CRediT roles: *Conceptualization, Methodology, Software, Formal analysis, Validation, Investigation, Resources, Data Curation, Writing – original draft, Writing – review & editing, Visualization, Project administration*) and holds complete scholarly, legal, and scientific responsibility for every claim, equation, program, formal lemma, and citation in this repository and accompanying monograph.
- **No unverified AI claim is trusted.** All 100 discrete Lean 4.35.0 software-model lemmas (`O01`–`O100`) are machine-checked by the Lean 4.35.0 reference kernel (`lean`, `leanchecker`, `leanchecker-paranoid`) plus four independent checkers (`lean4lean` via `lake check --paranoid`, Rust `nanoda_bin`, `con-leche --verified`, `con-ron --verified`), `lake comparator --paranoid`, a negative elaboration test, and live negative axiom-gate & comparator `sorry`-tamper rejection tests (`scripts/test_lean_negative.py`). Every numerical figure, SHA-256 digest, statistical p-value, citation check, and benchmark is emitted by live deterministic execution of the current repository commit.
- **Honest Non-Claims & Preprint Status:** This work is an independent research preprint and software archive and has **not** undergone external peer review. Cross-checking against `0thernes/unsolved-problems-mega-atlas` (`V2.6.0`) confirms that **`0 / 1,900`** overarching open problems are solved (`docs/mega-atlas-cross-check.md`).
