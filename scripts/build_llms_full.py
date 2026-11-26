#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2026 Alexander Stephen Donahue
"""Generate llms-full.txt concatenating core documentation for LLM ingestion (Build Standard A32)."""

from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

FILES = [
    "README.md",
    "AGENTS.md",
    "CLAIMS.md",
    "LIMITATIONS.md",
    "VERIFICATION.md",
    "PREREGISTRATION.md",
    "REPLICATION.md",
    "docs/mega-atlas-cross-check.md",
    "docs/statement-fidelity.md",
    "docs/DATASHEET.md",
    "docs/architecture.md",
    "docs/benchmarks.md",
    "docs/threat-model.md",
    "docs/AI_DISCLOSURE.md",
]


def main() -> int:
    version = (ROOT / "VERSION").read_text(encoding="utf-8").strip()
    parts = [f"# CELLULAR ENGINE OMEGA (v{version}) — Complete LLM Context Bundle\n"]
    included = 0
    for rel in FILES:
        p = ROOT / rel
        if p.exists():
            parts.append(f"\n---\n## FILE: {rel}\n---\n")
            parts.append(p.read_text(encoding="utf-8"))
            included += 1
    out = ROOT / "llms-full.txt"
    out.write_text("\n".join(parts), encoding="utf-8", newline="\n")
    print(f"Wrote {out.name} ({out.stat().st_size:,} bytes from {included} core docs).")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
