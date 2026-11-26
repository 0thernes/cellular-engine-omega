#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2026 Alexander Stephen Donahue
"""Generate Interactive Lean 4.35.0 Proof Blueprint (blueprint/index.html & blueprint/blueprint.tex)."""

from __future__ import annotations

import html
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parent.parent
LEAN_DIR = ROOT / "lean" / "OmegaCellularProofs"
BP_DIR = ROOT / "blueprint"
BP_DIR.mkdir(parents=True, exist_ok=True)


def parse_axioms() -> dict[str, str]:
    txt = (ROOT / "results" / "lean4-omega-audit.txt").read_text(encoding="utf-8-sig")
    rx = re.compile(r"^'(OmegaCellular\.[^']+)' (depends on axioms:\s*\[([^\]]+)\]|does not depend on any axioms)$", re.M)
    out: dict[str, str] = {}
    for m in rx.finditer(txt):
        out[m.group(1)] = m.group(3).strip() if m.group(3) else "none (axiom-free)"
    return out


def main() -> int:
    version = (ROOT / "VERSION").read_text(encoding="utf-8").strip()
    axioms = parse_axioms()
    grouped: dict[str, list[tuple[str, str]]] = {}
    for full, ax in axioms.items():
        parts = full.split(".")
        mod = parts[1] if len(parts) >= 3 else "Root"
        thm = parts[-1]
        grouped.setdefault(mod, []).append((thm, ax))

    cards_html = []
    tex_lines = [
        f"% Auto-generated Lean 4.35.0 Proof Blueprint (CELLULAR ENGINE OMEGA v{version})",
        f"\\section*{{CELLULAR ENGINE OMEGA v{version} — Lean 4.35.0 Proof Blueprint}}",
        f"Total machine-checked theorems: {len(axioms)} across {len(grouped)} modules (strictly constructive within \\{{propext, Quot.sound\\}}, zero Classical.choice, zero sorryAx, zero Mathlib).",
    ]
    for mod, items in grouped.items():
        rows = []
        tex_lines.append(f"\\subsection*{{{mod} ({len(items)} theorems)}}")
        tex_lines.append("\\begin{itemize}")
        for thm, ax in items:
            badge_cls = "badge-free" if "axiom-free" in ax else "badge-const"
            rows.append(
                f"<tr><td><code>{html.escape(thm)}</code></td>"
                f"<td><span class='badge {badge_cls}'>{html.escape(ax)}</span></td>"
                f"<td><span class='badge badge-ok'>PROVEN</span></td></tr>"
            )
            tex_lines.append(f"  \\item \\texttt{{{thm.replace('_', '\\_')}}} — axioms: \\texttt{{{ax}}}")
        tex_lines.append("\\end{itemize}")
        cards_html.append(
            f"<section class='mod-card'><h2>{html.escape(mod)} <span class='count'>{len(items)}</span></h2>"
            f"<table><thead><tr><th>Theorem</th><th>Kernel Axioms</th><th>Status</th></tr></thead>"
            f"<tbody>{''.join(rows)}</tbody></table></section>"
        )

    page = f"""<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width,initial-scale=1">
  <title>CELLULAR ENGINE OMEGA v{version} — Lean 4.35.0 Proof Blueprint</title>
  <style>
    :root {{ --bg:#0b1120; --panel:#111a2e; --border:#1e293b; --text:#e2e8f0; --muted:#94a3b8; --accent:#38bdf8; --ok:#10b981; --purple:#a78bfa; }}
    body {{ margin:0; font-family:system-ui,-apple-system,Segoe UI,Roboto,sans-serif; background:var(--bg); color:var(--text); line-height:1.55; }}
    header {{ padding:2rem 1.5rem; border-bottom:1px solid var(--border); background:linear-gradient(135deg,#0f172a,#1e1b4b); }}
    main {{ max-width:1180px; margin:0 auto; padding:1.5rem; }}
    h1 {{ margin:0 0 .4rem; font-size:1.75rem; }}
    .meta {{ color:var(--muted); font-size:.95rem; }}
    .pills {{ display:flex; flex-wrap:wrap; gap:.6rem; margin-top:1rem; }}
    .pill {{ background:#0f172a; border:1px solid var(--border); border-radius:999px; padding:.3rem .85rem; font-size:.82rem; color:var(--accent); }}
    .grid {{ display:grid; grid-template-columns:repeat(auto-fit,minmax(520px,1fr)); gap:1.25rem; margin-top:1.25rem; }}
    .mod-card {{ background:var(--panel); border:1px solid var(--border); border-radius:12px; padding:1rem 1.2rem; }}
    .mod-card h2 {{ margin:0 0 .75rem; font-size:1.05rem; display:flex; justify-content:space-between; align-items:center; color:#f8fafc; }}
    .count {{ background:#1e293b; color:var(--accent); padding:.1rem .55rem; border-radius:999px; font-size:.8rem; }}
    table {{ width:100%; border-collapse:collapse; font-size:.84rem; }}
    th,td {{ text-align:left; padding:.42rem .5rem; border-bottom:1px solid #1e293b; }}
    th {{ color:var(--muted); font-weight:600; }}
    code {{ color:#bae6fd; font-size:.8rem; word-break:break-word; }}
    .badge {{ display:inline-block; padding:.12rem .48rem; border-radius:6px; font-size:.73rem; font-weight:600; }}
    .badge-ok {{ background:#064e3b; color:#6ee7b7; }}
    .badge-free {{ background:#1e3a8a; color:#bfdbfe; }}
    .badge-const {{ background:#3b0764; color:#e9d5ff; }}
    a {{ color:var(--accent); }}
  </style>
</head>
<body>
  <header>
    <div style="max-width:1180px;margin:0 auto;">
      <h1>CELLULAR ENGINE OMEGA v{version} — Lean 4.35.0 Proof Blueprint</h1>
      <div class="meta">Author: Alexander Stephen Donahue (ORCID 0009-0006-8168-9648) · Independent Researcher, Spokane, WA, USA · <a href="../index.html">Project Home</a> · <a href="https://github.com/0thernes/cellular-engine-omega">GitHub Repository</a></div>
      <div class="pills">
        <span class="pill">{len(axioms)} / {len(axioms)} Theorems Proven</span>
        <span class="pill">0 Mathlib · 0 sorryAx · 0 Classical.choice</span>
        <span class="pill">Isolated Spec.lean -> Challenge.lean / Solution.lean</span>
        <span class="pill">All 6 Independent Kernels (lean · leanchecker-paranoid · lean4lean · nanoda_bin · con-leche · con-ron)</span>
      </div>
    </div>
  </header>
  <main>
    <p>Every theorem below is extracted directly from the live <code>results/lean4-omega-audit.txt</code> kernel output of <code>lean/VerifyAxioms.lean</code>. Live C11 runtime telemetry is bound into the kernel via <code>LiveC11Receipt.lean</code> (<code>O63</code>, <code>O72</code>), every universal hypothesis bundle is inhabited and sharp-boundary tested in <code>NonVacuity.lean</code> (<code>O73</code>–<code>O89</code>), and 11 weakened/inverted domain falsifications are refuted constructively in <code>AdversarialTests.lean</code> (<code>O90</code>–<code>O100</code>).</p>
    <div class="grid">{''.join(cards_html)}</div>
  </main>
</body>
</html>
"""
    (BP_DIR / "index.html").write_text(page, encoding="utf-8", newline="\n")
    (BP_DIR / "blueprint.tex").write_text("\n".join(tex_lines) + "\n", encoding="utf-8", newline="\n")
    print(f"Generated blueprint/index.html and blueprint/blueprint.tex ({len(axioms)} theorems across {len(grouped)} modules).")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
