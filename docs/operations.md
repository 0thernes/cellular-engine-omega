# Operations, Backups, Archival & Offsite Mirror (`A35`)

## Local Web Observatory Operations

Start the hardened loopback server:
```bash
node web/server.mjs
```
- **Health probes (`A34`):** `GET http://127.0.0.1:8787/api/health` and `GET http://127.0.0.1:8787/healthz` return JSON `{ ok, version, lean_theorems, lean_kernels, gates, mutants, substrates_75, hybrid_cube_1000, inflight, uptime_s }`.
- **Structured logs (`A34`):** Every request and error emits single-line JSON to `stdout`/`stderr` (`ts`, `level`, `method`, `path`, `status`, `ms`).
- **Concurrency protection:** `MAX_INFLIGHT = 2` caps concurrent `omega_cell` subprocess invocations (excess requests receive HTTP `429`).

## Static Deployment & Rollback (`A36`)

- **GitHub Pages (`pages/` + `blueprint/`):** Published automatically on pushes to `main` via `.github/workflows/pages.yml` (`https://0thernes.github.io/cellular-engine-omega/`).
- **One-command rollback:** `just rollback v5.1.0` restores `pages/` and `dist/` from a previous release tag.

## Data Integrity, Git Bundles & Offsite Mirroring (`A35`)

1. **Cryptographic checksums:** `python3 scripts/generate_checksums.py --verify` validates every frozen receipt, dataset CSV, white paper PDF, arXiv tarball, and Lean contract in `CHECKSUMS.sha256` and `data/MANIFEST.sha256`.
2. **Offline Git bundle backup:** Create a self-contained verifiable archive of all commits and tags:
   ```bash
   git bundle create ../cellular-engine-omega-v5.2.0.bundle --all
   git bundle verify ../cellular-engine-omega-v5.2.0.bundle
   ```
3. **Offsite permanent archival mirrors:**
   - **Primary remote:** `https://github.com/0thernes/cellular-engine-omega`
   - **Master Research Archive submodule mirror (`0TH-BUILD-008`):** `https://github.com/0thernes/0thernes-research-archive`
   - **Software Heritage permanent archival snapshot:** Submitted via Software Heritage Save Code Now API (`https://archive.softwareheritage.org/api/1/origin/save/git/url/https://github.com/0thernes/cellular-engine-omega`) for immutable SWHID preservation.
   - **Local secondary drive mirror:** Mirrored from ReFS Dev Drive (`Z:\[Vibe Coded (AI)]\ANTIGRAVITY\cellular-engine-omega`) to archival volume `I:\` via SHA-256 verified synchronization.
