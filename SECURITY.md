# Security Policy

## Supported Versions

| Version | Supported |
| :--- | :--- |
| `5.2.x` | Supported |
| `< 5.2.0` | End of life (upgrade to `5.2.0`) |

## Security Architecture & Hardening Posture

CELLULAR ENGINE OMEGA is designed as an offline, loopback-only, zero-dependency scientific simulation and formal-verification toolkit:
- **Native C11 engine (`bin/omega_cell`, `bin/libomega_cell`)**: Zero network sockets, zero dynamic `eval`, strict CLI integer parsing (`parse_u64_strict`, `parse_mutant_strict`), compiled with `-Wall -Wextra -Wshadow -Werror`, verified via `test/fuzz_omega_cli_codec.c` (`256`-case mutated byte-stream harness + libFuzzer target) and Linux ASan/UBSan in CI.
- **Hardened Web Observatory (`web/server.mjs`)**: Binds strictly to `127.0.0.1`, enforces loopback `Host` / `Origin` / `Sec-Fetch-Site` checks (`403` on cross-origin/DNS-rebinding attempts), serves only `ALLOWED_STATIC = {'index.html', 'app.js', 'styles.css'}` (`GET /server.mjs -> 404`), sets strict CSP (`object-src 'none'; frame-ancestors 'none'`) + `X-Content-Type-Options: nosniff` + `X-Frame-Options: DENY`, rejects non-GET/HEAD methods (`405`), bounds concurrent subprocess runs (`MAX_INFLIGHT = 2`, returning `429`), and uses `execFile` (never shell interpolation).

## Reporting a Vulnerability

Please report security issues privately by emailing **0_0@0thernes.art** with subject `[SECURITY] cellular-engine-omega: <brief summary>`, including reproduction steps and affected commit SHA or version. Do not open public GitHub issues for undisclosed vulnerabilities. Target initial acknowledgment is within 72 hours.
