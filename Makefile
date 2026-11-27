# SPDX-License-Identifier: Apache-2.0
# CELLULAR ENGINE OMEGA v5.4.0 — GNU Make fallback alongside justfile

.PHONY: all build test lint bench run reproduce fuzz sanitize lean paper audit clean

all: build

build:
	./build_linux.sh

test:
	./bin/test_omega_falsification
	./bin/fuzz_omega_standalone
	python3 -m pytest tests/ -v

lint:
	node --check web/server.mjs
	node --check web/app.js
	python3 scripts/verify_version_sync.py
	python3 scripts/check_readme_axioms.py
	python3 scripts/check_citations.py

bench:
	python3 scripts/run_stats_and_benchmarks.py

run:
	./bin/omega_cell all --seed 2026001
	./bin/omega_cell cube results/omega_75_substrates_1000_cube.json --seed 2026001

reproduce: build test lint
	python3 scripts/generate_checksums.py --verify

fuzz:
	./bin/fuzz_omega_standalone

sanitize:
	OMEGA_SANITIZE=1 ./build_linux.sh

lean:
	cd lean && lake build && lake build Challenge && lake env lean VerifyAxioms.lean > ../results/lean4-omega-audit.txt && lake comparator --config comparator.json --paranoid

paper:
	python3 scripts/generate_figures.py
	cd paper && tectonic -X compile main.tex --outdir build --keep-logs --keep-intermediates
	cp paper/build/main.pdf dist/Donahue_2026_cellular_engine_omega.pdf

audit:
	./scripts/audit-repo.sh

clean:
	rm -rf bin/*.o paper/build
