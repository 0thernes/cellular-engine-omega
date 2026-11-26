# Independent Replication Protocol & Log (`B43` / `B44`)

This document provides a self-contained protocol for replicating every build, C11 falsification gate, Python verification test, multi-seed statistical claim, Lean 4.35.0 multi-kernel proof, and AMS-LaTeX white paper PDF from a **clean clone** in an empty directory.

---

## 1. Prerequisites

| Tool | Required Version | Purpose |
| :--- | :--- | :--- |
| C11 Compiler | GCC `>= 11` or Clang `>= 16` | Build `bin/omega_cell`, `test_omega_falsification`, `fuzz_omega_standalone` |
| Lean 4 (`elan`) | `leanprover/lean4:v4.35.0` (pinned in `lean/lean-toolchain`) | Build `OmegaCellularProofs` / `Challenge` / `Solution` and run multi-kernel audits |
| Python | `>= 3.11` with `pytest`, `hypothesis`, `jsonschema`, `numpy`, `scipy`, `matplotlib`, `pyyaml` | Run verification ladder, exact oracle, multi-seed stats, and figures |
| Node.js | `>= 20` | Verify JSON outputs and run loopback Web Observatory smoke tests |
| Tectonic *(optional, for paper)* | `>= 0.15` | Compile `paper/main.tex` -> `dist/Donahue_2026_cellular_engine_omega.pdf` |

---

## 2. Step-by-Step Clean-Clone Protocol

### Linux / macOS
```bash
git clone https://github.com/0thernes/cellular-engine-omega.git omega-repro
cd omega-repro
# 1. C11 build + ASan/UBSan + 60 gates + 16/16 mutants + 256 fuzz + Live C11->Lean export + 100 Lean proofs
OMEGA_SANITIZE=1 ./build_linux.sh
# 2. All 6 independent Lean 4.35.0 kernels + live negative kernel test + isolated Challenge/Solution comparator
./scripts/verify_lean_six_kernels.sh
# 3. Independent Python verification ladder + exact oracle + Hypothesis (1,200+ cases) + negative Lean test
python3 -m pytest tests/ -v
# 4. Version, README axiom-block, live Crossref citation, FAIR metadata, and SHA-256 checksum audits
python3 scripts/verify_version_sync.py
python3 scripts/check_readme_axioms.py
python3 scripts/check_citations.py
python3 scripts/generate_fair_metadata.py --verify
python3 scripts/generate_checksums.py --verify
# 5. Build Standard v1.2.0 repository audit
./scripts/audit-repo.sh
```

### Windows (PowerShell 7+)
```powershell
git clone https://github.com/0thernes/cellular-engine-omega.git omega-repro
cd omega-repro
pwsh -File build.ps1
python -m pytest tests/ -v
python scripts/verify_version_sync.py
python scripts/check_readme_axioms.py
python scripts/check_citations.py
python scripts/generate_checksums.py --verify
pwsh -File scripts/Audit-Repo.ps1
```

---

## 3. Expected Deterministic Receipts (`v5.3.0`, Seed `2026001`, x86-64 `-ffp-contract=off`)

- **Falsification Suite:** `60 / 60 PASSED · 0 FAILED · 16/16 MUTANTS CAUGHT`
- **Fuzz + Property Harness:** `256 / 256` mutated cases + `32 / 32` property seeds PASS (`0` crashes)
- **Master SHA-256 (`TB01`–`TB16`):**
  - Windows x86-64 (Clang / UCRT `libm`, `-O0` == `-O3`): `a92641843e46736b04cd781181f34f1de8444859ce73db47d4204c5553364661`
  - Ubuntu 26.04 x86-64 (GCC / `glibc libm`): `48eb8e8d2638d1f2a95f9f1add672d27a1c63f76c2b69e0b8318a3135e324238` *(transcendental `libm` ULP difference; all integer/topological invariants & all 100 Lean proofs match identically)*
- **75-Substrate / 1,000-Cube SHA-256:**
  - Windows x86-64 (Clang / UCRT): `c9fd6d3dfcf1492c2271e2fd7a6e30473ff02187ca17a9d664493df0fb0a55ab`
  - Ubuntu 26.04 x86-64 (GCC / `glibc`): `403258d5ded2d7ac96566ac4147a029dd1858a68ac8096128691f5fd970103db`
- **Live C11 Receipt SHA-256 (`LiveC11Receipt.lean`):**
  - Windows x86-64: `12d58fa590f59aa052f243fcb0ea8b0d24c41b6fff55185c3671d0bc1b0941c2` · Ubuntu 26.04 x86-64: `cd6cc557b4a0a2fccb4d77bcca47cba8156510fea8b5bd758dbeb023b94e13fd` *(both satisfy `O63` & `O72` inside all 6 Lean 4.35.0 kernels)*
- **Lean 4.35.0 Formal Proofs:** `100 / 100` theorems strictly within `{propext, Quot.sound}` (`0` `Classical.choice`, `0` `sorryAx`, `0` warnings) across all `6` independent kernels + live negative kernel rejection test

---

## 4. Executed Independent Replication Log (`B44`)

*Submit independent replications via the `Independent Replication Report` issue template.*

| Date (UTC) | Replicator | OS / Arch / Toolchain | Commit / Tag | Outcome | Observed Master SHA-256 / Notes |
| :--- | :--- | :--- | :--- | :--- | :--- |
| 2026-11-25 | Author clean-directory replication (fresh temp clone outside workspace) | Windows 11 x86_64 · Clang (Lean 4.35.0 bundle) · Lean 4.35.0 · Python 3.12 · Node 22 | `v5.2.0` (`f6bb7ec`) | **PASS** | `a92641843e46736b...` · `60/60` gates, `16/16` mutants, `89/89` Lean proofs (`lean`, `leanchecker`, `leanchecker-paranoid`, `con-leche`, `con-ron`), `pytest` PASS, `Audit-Repo.ps1` `100.0%` |
| 2026-11-25 | GitHub Actions Multi-OS CI Run [`#45749131672`](https://github.com/0thernes/cellular-engine-omega/actions/runs/45749131672) | Ubuntu 24.04 x86_64 (GCC + ASan/UBSan + Lean 4.35.0 `lake comparator --paranoid`) · macOS ARM64 (Clang) · Windows x64 (MinGW GCC) | `v5.2.0` (`f6bb7ec`) | **PASS (ALL 3 OS GREEN)** | Full Linux ELF64 + ASan/UBSan + `60/60` gates + `16/16` mutants + `lake comparator --paranoid` + Web Observatory API + SLSA build provenance + GitHub Release [`#45749136562`](https://github.com/0thernes/cellular-engine-omega/actions/runs/45749136562) |
| 2026-11-26 | Author Round-2 verification (`v5.3.0`) | Windows 11 x86_64 · Clang + Lean 4.35.0 (`lean`, `leanchecker`, `leanchecker-paranoid`, `con-leche`, `con-ron`, `nanoda_bin` + negative kernel test) · Python 3.12 · Tectonic | `v5.3.0` | **PASS** | `a92641843e46736b...` · `60/60` gates, `16/16` mutants, `100/100` constructive Lean proofs (`0` `Classical.choice`), `32/32` live Crossref DOIs (`0` retractions), FAIR Croissant/RO-Crate/CodeMeta, RFC 3161 DigiCert `.tsr`, `Audit-Repo.ps1` `100.0%` |
