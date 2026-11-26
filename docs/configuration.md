# Configuration & Environment Reference (`A33`)

CELLULAR ENGINE OMEGA (`v5.2.0`) requires **zero secrets, zero external API keys, and zero network services** at runtime. All configuration is passed via explicit CLI flags or optional environment variables (see `.env.example`).

## Environment Variables

| Variable | Default | Component | Description |
| :--- | :--- | :--- | :--- |
| `PORT` | `8787` | `web/server.mjs` | Local TCP port on `127.0.0.1` for the Web Observatory |
| `HOST` | `127.0.0.1` | `web/server.mjs` | Loopback bind address (non-loopback binding is intentionally disallowed) |
| `CC` | `gcc` | `build_linux.sh` | C11 compiler binary (`gcc` or `clang`) |
| `OMEGA_SANITIZE` | `0` | `build_linux.sh` | Set to `1` to compile and run ASan + UBSan binaries (`-fsanitize=address,undefined`) |
| `OMEGA_URL` | `http://127.0.0.1:8787` | `examples/web_api_demo.mjs` | Base URL for the Node.js Web API example client |

## CLI Flags (`bin/omega_cell`)

- `omega_cell all [--seed <u64>=2026001] [--mutant <0..16>=0]`: Run all 16 testbeds with human-readable telemetry and Master SHA-256.
- `omega_cell json [--seed <u64>=2026001] [--mutant <0..16>=0]`: Emit schema-validated JSON (`schema/omega_run.schema.json`).
- `omega_cell cube <out.json> [--seed <u64>=2026001]`: Run Clean-Slate v3.0 75 Substrates + 1,000-Cell Hybrid Cube (`schema/omega_cube.schema.json`).
- `omega_cell --export-lean-trace <out.lean> [--seed <u64>=2026001]`: Emit live C11 telemetry & SHA-256 receipt to `lean/OmegaCellularProofs/LiveC11Receipt.lean`.
- `omega_cell --version` / `--help`: Print version (`5.2.0`) or usage guide. Invalid arguments exit with status code **`2`**.

## Path Notes (Windows ReFS Dev Drive)

- When running under bracketed directories (e.g., `Z:\[Vibe Coded (AI)]\ANTIGRAVITY\cellular-engine-omega`), always pass `-LiteralPath` to PowerShell file cmdlets and invoke `pytest` from inside the repository root using relative paths (`pytest tests/`).
