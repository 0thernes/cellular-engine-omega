# SPDX-License-Identifier: Apache-2.0
# CELLULAR ENGINE OMEGA v5.4.0 — Task Runner (Build Standard v1.2.0: A31, A36, B01)
# Zero git hooks; every recipe is explicit and directly invocable.

set windows-shell := ["pwsh.exe", "-NoProfile", "-Command"]

default:
    @just --list

setup:
    @echo "Toolchains required: Clang/GCC (C11), Lean 4.35.0 (elan), Node.js 20+, Python 3.11+ (pytest, hypothesis, scipy, matplotlib, jsonschema, pyyaml), Tectonic (AMS-LaTeX)."

build:
    #!/usr/bin/env bash
    set -euo pipefail
    ./build_linux.sh

test:
    #!/usr/bin/env bash
    set -euo pipefail
    ./bin/test_omega_falsification
    ./bin/fuzz_omega_standalone
    python3 -m pytest tests/ -v

lint:
    #!/usr/bin/env bash
    set -euo pipefail
    node --check web/server.mjs
    node --check web/app.js
    python3 scripts/verify_version_sync.py
    python3 scripts/check_readme_axioms.py
    python3 scripts/check_citations.py
    if command -v cppcheck >/dev/null 2>&1; then cppcheck --enable=warning,performance,portability --std=c11 -Iinclude src test || true; fi

bench:
    #!/usr/bin/env bash
    set -euo pipefail
    python3 scripts/run_stats_and_benchmarks.py

run:
    #!/usr/bin/env bash
    set -euo pipefail
    ./bin/omega_cell all --seed 2026001
    ./bin/omega_cell cube results/omega_75_substrates_1000_cube.json --seed 2026001

reproduce:
    #!/usr/bin/env bash
    set -euo pipefail
    ./build_linux.sh
    python3 -m pytest tests/ -v
    python3 scripts/verify_version_sync.py
    python3 scripts/check_readme_axioms.py
    python3 scripts/generate_checksums.py --verify

fuzz:
    #!/usr/bin/env bash
    set -euo pipefail
    ./bin/fuzz_omega_standalone

sanitize:
    #!/usr/bin/env bash
    set -euo pipefail
    OMEGA_SANITIZE=1 ./build_linux.sh

lean:
    #!/usr/bin/env bash
    set -euo pipefail
    chmod +x scripts/verify_lean_six_kernels.sh
    ./scripts/verify_lean_six_kernels.sh

fair:
    #!/usr/bin/env bash
    set -euo pipefail
    python3 scripts/generate_fair_metadata.py
    python3 scripts/generate_checksums.py --verify

paper:
    #!/usr/bin/env bash
    set -euo pipefail
    python3 scripts/generate_figures.py
    mkdir -p paper/build
    (cd paper && tectonic -X compile main.tex --outdir build --keep-logs --keep-intermediates)
    cp paper/build/main.pdf dist/Donahue_2026_cellular_engine_omega.pdf

audit:
    #!/usr/bin/env bash
    set -euo pipefail
    ./scripts/audit-repo.sh

clean:
    #!/usr/bin/env bash
    set -euo pipefail
    rm -rf bin/*.o paper/build lean/.lake/build

rollback tag="v5.2.0":
    @echo "To roll back GitHub Pages or release artifacts to {{tag}}, run: git checkout {{tag}} -- pages/ dist/ && git commit -m 'revert: rollback static site to {{tag}}' && git push origin main"
