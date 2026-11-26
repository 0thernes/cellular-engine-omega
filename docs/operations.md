# Operations, Backups, Archival & Offsite Mirror (`A35`)

## Local Web Observatory Operations

Start the hardened loopback server:
```bash
node web/server.mjs
```
- **Health probes (`A34`):** `GET http://127.0.0.1:8791/api/health` and `GET http://127.0.0.1:8791/healthz` return JSON `{ ok, version, port, testbeds, substrates_75, hybrid_cube_1000, lean_theorems, lean_kernels, gates, mutants }`.
- **Structured logs (`A34`):** Every API request, rejection, and error emits single-line JSON to `stdout`/`stderr` (`ts`, `level`, `event`, `version`, `status`, `duration_ms`).
- **Concurrency protection:** `MAX_INFLIGHT = 2` caps concurrent `omega_cell` subprocess invocations (excess requests receive HTTP `429`).

## Static Deployment & Rollback (`A36`)

- **GitHub Pages (`pages/` + `blueprint/` + FAIR metadata):** Published automatically on pushes to `main` via `.github/workflows/pages.yml` (`https://0thernes.github.io/cellular-engine-omega/`).
- **One-command rollback:** `just rollback v5.2.0` restores `pages/` and `dist/` from a previous release tag.

## Data Integrity, RFC 3161 Timestamps, Git Bundles & Offsite Mirroring (`A35`)

1. **Cryptographic checksums & RFC 3161 timestamps:** `python3 scripts/generate_checksums.py --verify` validates every frozen receipt, dataset CSV, FAIR manifest, white paper PDF, arXiv tarball, and Lean contract in `CHECKSUMS.sha256` and `data/MANIFEST.sha256`. DigiCert RFC 3161 `.tsr` cryptographic timestamps live under `timestamps/`.
2. **Offline Git bundle backup:** Create a self-contained verifiable archive of all commits and tags:
   ```bash
   git bundle create ../cellular-engine-omega-v5.3.0.bundle --all
   git bundle verify ../cellular-engine-omega-v5.3.0.bundle
   ```
3. **Offsite permanent archival mirrors:**
   - **Primary remote:** `https://github.com/0thernes/cellular-engine-omega`
   - **Master Research Archive submodule mirror (`0TH-BUILD-008`):** `https://github.com/0thernes/0thernes-research-archive`
   - **Software Heritage permanent archival snapshot:** Submit via browser at `https://archive.softwareheritage.org/save/` (origin `https://github.com/0thernes/cellular-engine-omega`) for immutable SWHID preservation.
   - **Local secondary drive mirror:** Mirrored from ReFS Dev Drive (`Z:\[Vibe Coded (AI)]\ANTIGRAVITY\cellular-engine-omega`) to archival volume `I:\` via SHA-256 verified synchronization.
