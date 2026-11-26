// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2026 Alexander Stephen Donahue
// Minimal Node.js client querying the hardened loopback Web Observatory API.
// Start server first: `node web/server.mjs` (default http://127.0.0.1:8787)

const BASE = process.env.OMEGA_URL || "http://127.0.0.1:8787";

async function main() {
  const health = await (await fetch(`${BASE}/api/health`)).json();
  console.log("Health:", JSON.stringify(health));
  const cube = await (await fetch(`${BASE}/api/cube?seed=2026001`)).json();
  console.log(
    `Substrates: ${cube.substrates_75.total_passed}/75 | Cube: ${cube.hybrid_cube_1000.ok} OK, ${cube.hybrid_cube_1000.adapt} ADAPT, ${cube.hybrid_cube_1000.over} OVER, ${cube.hybrid_cube_1000.excl} EXCL`
  );
}

main().catch((err) => {
  console.error(err);
  process.exit(1);
});
