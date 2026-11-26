# Third-Party Notices

**CELLULAR ENGINE OMEGA v5.2.0** is a clean-room, zero-runtime-dependency C11 engine and zero-Mathlib Lean 4.35.0 formalization. No third-party source code libraries are vendored into the runtime binaries (`omega_cell`, `libomega_cell`, `test_omega_falsification`, `fuzz_omega_standalone`) or web observatory (`web/server.mjs`, `web/app.js`).

## Build, Verification & Publishing Toolchains (Not Bundled)

| Component | Purpose | License | Upstream |
| :--- | :--- | :--- | :--- |
| **Lean 4 (`v4.35.0`) & Lake** | Formal proof assistant, `leanchecker`, `leanchecker-paranoid`, `leanexport`, `lean4lean`, `nanoda_bin`, `con-leche`, `con-ron` | Apache-2.0 | https://github.com/leanprover/lean4 |
| **LLVM / Clang & GCC** | Native C11 compilers (`-O3 -std=c11 -Wall -Wextra -Wshadow -Werror`) | Apache-2.0 with LLVM Exception / GPL-3.0-or-later with GCC Runtime Exception | https://llvm.org · https://gcc.gnu.org |
| **Node.js (`>= 20`)** | Zero-dependency HTTP/Canvas observatory server (`web/server.mjs`) | MIT | https://nodejs.org |
| **Tectonic** | Reproducible XeTeX/AMS-LaTeX white-paper compiler (`paper/main.tex`) | MIT | https://tectonic-typesetting.github.io |
| **Python 3 (`>= 3.10`)** | Standard-library build, citation, and axiom verification scripts (`scripts/*.py`) | PSF-2.0 | https://www.python.org |
| **Build Standard Audit Scripts** | `scripts/Audit-Repo.ps1`, `scripts/audit-repo.sh`, `scripts/audit-checks.tsv`, `repo-standard.schema.json` (Build Standard for LLMs v1.2.0) | Apache-2.0 | Local Build Standard template |

## Algorithms Implemented Clean-Room from Public Specifications
- **FIPS 180-4 SHA-256** (`src/omega_rng_hash.c`): Implemented clean-room from NIST FIPS PUB 180-4, verified bit-exact against official NIST test vectors in Gate `G58`.
- **Philox4x32-10 Counter-Based PRNG** (`src/omega_rng_hash.c`): Implemented clean-room from Salmon et al. (SC'11), deterministic across platforms.
- **OCP Microscaling FP4/FP8, IEEE-754 binary16/bfloat16, NF4, Posit16, LNS-16** (`src/omega_substrates_cube.c`): Implemented clean-room from public numeric format specifications.
