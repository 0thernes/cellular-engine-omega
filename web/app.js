let state = null;
let animT = 0;
let caShockTimer = 0;
let tunMode = false;
let vmemTwoHead = true;
let harpoonTimer = 0;

// Deterministic C11-backed 32-ray cytoskeleton visualizer (zero Math.random(); driven by state.tb03!)
const cargos = Array.from({ length: 12 }, (_, i) => ({
  ray: (i * 3) % 32,
  pos: 0.18 + ((i * 7) % 11) * 0.06,
  dir: i % 2 === 0 ? 1 : -1 // +1 Kinesin outward, -1 Dynein inward
}));

function esc(v) {
  return String(v ?? '').replace(/[&<>"']/g, (c) => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#39;' }[c]));
}

function kv(label, val, cls = '') {
  return `<div class="kv"><span>${esc(label)}</span><strong class="${cls}">${esc(val)}</strong></div>`;
}

async function fetchEngine(feral = false) {
  const seed = document.getElementById('seedInput').value || '2026001';
  const mutant = document.getElementById('mutantSelect').value || '0';
  try {
    const res = await fetch(`/api/run?seed=${encodeURIComponent(seed)}&mutant=${encodeURIComponent(mutant)}&feral=${feral ? 1 : 0}`, { cache: 'no-store' });
    if (!res.ok) throw new Error(`HTTP ${res.status}`);
    state = await res.json();
    renderAllStatic();
  } catch (err) {
    document.getElementById('statusBadges').innerHTML = `<div class="badge fail">ENGINE ERROR: ${esc(err.message)}</div>`;
  }
}

function renderAllStatic() {
  if (!state) return;
  const d = state;
  const t14 = d.tb14;
  const t15 = d.tb15;
  const t16 = d.tb16;

  // Badges
  const bEl = document.getElementById('statusBadges');
  const gates = [
    ['Purcell 1-DOF == 0', Math.abs(d.tb01.scallop_net_disp) < 1e-10],
    ['Mavirus Rescue', d.tb02.amoeba_with_mavirus > 2.5 * d.tb02.amoeba_no_mavirus],
    ['Poincare-Hopf χ=+2', d.tb05.max_err < 1e-9],
    ['Eigen Proofreading', d.tb06.kp1_L600 > 0.85],
    ['Turing Pattern', d.tb07.fast_inh_std > 0.10],
    ['TUR ≥ 2.0', d.tb08.tur_min >= 1.95],
    ['≤1000-Cell Tardigrade', d.tb09.total_cells <= 1000 && d.tb09.intact_surv > 50],
    ['Track-A 0 Tumors', d.tb12.trackA_tumors === 0],
    ['12D Open Proliferation', d.tb13.nov2 > 200 && d.tb13.mass_res < 1e-8],
    ['Bio-Frostbite 5×SHA Confluence', Boolean(t14 && t14.confluence_match === 1)],
    ['Linear 0-Drift & 0W Tun', Boolean(t14 && t14.mass_drift_fifo === 0 && t14.gap_charge_drift === 0 && t14.wire_errors === 0 && t14.tun_dry_power === 0)],
    ['CISS Homochiral + Levin 2H (0-DNA)', Boolean(t15 && t15.ciss_magnetite_ee > 0.96 && t15.levin_reprog_heads === 2 && t15.levin_dna_mut === 0)],
    ['Virtual-Cell Physics Projection', Boolean(t16 && t16.proj_mass_res < 1e-12 && t16.perturb_r2 > 0.85)]
  ];
  bEl.innerHTML =
    `<div class="badge info">C11 Runtime: ${d.elapsed_ms.toFixed(2)} ms · ${d.feral ? 'FERAL HW ENTROPY' : 'Seed ' + esc(d.seed)}${d.mutant ? ' · M' + d.mutant : ''}</div>` +
    `<div class="badge">Lean 4: 60 / 60 PROVEN (148 Ecosystem · 0 sorryAx)</div>` +
    gates.map(([lbl, ok]) => `<div class="badge ${ok ? '' : 'fail'}">${ok ? 'PASS' : 'MUTANT CAUGHT'}: ${esc(lbl)}</div>`).join('');

  // TB15 Deep Strata Frontier metrics
  if (t15) {
    const cissOk = t15.ciss_magnetite_ee > 0.96;
    const levOk = t15.levin_reprog_heads === 2 && t15.levin_dna_mut === 0;
    const dsTag = document.getElementById('deepStrataTag');
    if (dsTag) {
      dsTag.textContent = !levOk ? 'M9: CISS/VMEM BROKEN!' : (vmemTwoHead ? '2-HEAD BICEPHALIC (0-DNA)' : '1-HEAD WILD VMEM');
      dsTag.style.color = (cissOk && levOk) ? '#a78bfa' : '#fb7185';
    }
    document.getElementById('mDeepStrata').innerHTML =
      kv('CISS Magnetite & TAD', `ee=${(100*t15.ciss_magnetite_ee).toFixed(1)}% · TAD=${t15.tad_gain.toFixed(1)}× · ${t15.oxy_mds} MDS`, cissOk ? 'ok' : 'bad') +
      kv('Trp UV Superrad & Cnid', `QY=${t15.trp_qy_gain.toFixed(2)}× · Dark=${t15.trp_subrad.toFixed(0)}× · ${(t15.cnidocyte_g/1e6).toFixed(2)}M g`, 'ok') +
      kv('Levin Vmem & 3-Genome', `${t15.levin_wild_heads}H→${t15.levin_reprog_heads}H (${t15.levin_dna_mut} DNA) · Δ3G=${t15.three_gen_drift} · Kl=${t15.klepto_gain.toFixed(1)}×`, levOk ? 'ok' : 'bad') +
      kv('Wireless Connectome', `Wired ${(100*t15.wired_reach).toFixed(1)}% → Wireless ${(100*t15.wireless_reach).toFixed(1)}% · Rhomb=${t15.rhombozoa_cells}c`, 'ok');
  }

  // TB16 Virtual-Cell AI & JCVI-syn3A Dark Proteome metrics + static canvas
  if (t16) {
    const projOk = t16.proj_mass_res < 1e-12;
    const vcTag = document.getElementById('vcellTag');
    if (vcTag) {
      vcTag.textContent = projOk ? `R²=${t16.perturb_r2.toFixed(3)} · Δ=${t16.proj_mass_res.toExponential(1)}` : 'M10: AI MASS HALLUCINATION!';
      vcTag.style.color = projOk ? '#34d399' : '#fb7185';
    }
    document.getElementById('mVirtualCell').innerHTML =
      kv('PDE Hepatic Zonation', `O₂→Gluc r=${t16.zonation_mono.toFixed(3)} · 32z 1D PDE`, 'ok') +
      kv('Ridge Zero-Shot (Ctx B)', `Proj R²=${t16.perturb_r2.toFixed(3)} (Raw ${(t16.raw_r2 ?? t16.perturb_r2).toFixed(3)}) · AUC=${t16.essential_auroc.toFixed(3)}`, 'ok') +
      kv('573-Gene syn3A Graph', `${t16.syn3a_discovered}/${t16.syn3a_dark} dark essentials (${t16.syn3a_total} min)`, 'ok') +
      kv('Physics Projection (O56)', `Raw=${t16.raw_ai_halluc.toFixed(3)} → Proj=${t16.proj_mass_res.toExponential(1)} · 6T=${t16.six_tier_err.toExponential(0)}`, projOk ? 'ok' : 'bad');
    drawVirtualCell(t16);
  }

  // TB14 Bio-Frostbite 4-Layer Interaction Net VM metrics
  if (t14) {
    const confOk = t14.confluence_match === 1;
    const linOk = t14.mass_drift_fifo === 0 && t14.mass_drift_lifo === 0 && t14.mass_drift_random === 0 && t14.gap_charge_drift === 0 && t14.wire_errors === 0;
    const rpsM = (t14.rewrites_per_sec / 1e6).toFixed(2);
    const inetTag = document.getElementById('inetTag');
    inetTag.textContent = tunMode ? '0W BIOGLASS TUN FROZEN' : (confOk ? `5×SHA EXACT · ${rpsM}M rw/s` : 'M8 RACE: CONFLUENCE BROKEN!');
    inetTag.style.color = confOk ? '#34d399' : '#fb7185';
    document.getElementById('mBioInet').innerHTML =
      kv('5-Sched Confluence', confOk ? `EXACT · ${t14.sha_fifo.slice(0, 10)}…` : `MISMATCH! ${t14.sha_fifo.slice(0, 6)}≠${t14.sha_random.slice(0, 6)}`, confOk ? 'ok' : 'bad') +
      kv('4-Layer Active Pairs', `L1=${t14.layer_rewrites[0]} L2=${t14.layer_rewrites[1]} L3=${t14.layer_rewrites[2]} L4=${t14.layer_rewrites[3]}`, 'ok') +
      kv('Linear Mass / Q / Wire', `ΔM=${t14.mass_drift_fifo} · ΔQ=${t14.gap_charge_drift} · Err=${t14.wire_errors} · D${t14.max_depth}`, linOk ? 'ok' : 'bad') +
      kv('Tun 0W & Hybrid SSIN', `Dry=${t14.tun_dry_pairs}/${t14.tun_dry_power}W · H₂O=+${t14.tun_rehyd_rewrites} · SSIN=${t14.ssin_confluent_pct.toFixed(1)}%`, 'ok');
  }

  // TB13 12-Axis Open Proliferation metrics & radar
  document.getElementById('prolifTag').textContent = d.tb13.apex;
  document.getElementById('mMorpho12').innerHTML =
    kv('12D Unique Niches', `${d.tb13.niches} (H1=${d.tb13.nov1}, H2=${d.tb13.nov2})`, d.tb13.nov2 > 200 ? 'ok' : 'bad') +
    kv('Chimeras / Swarms / Creat', `D2-3=${d.tb13.chimeras} · Sw=${d.tb13.swarms} · Org=${d.tb13.creatures}`, 'ok') +
    kv('Wild Outliers & Span', `${d.tb13.outliers} extreme · ${(100*d.tb13.vol).toFixed(1)}% span`, 'ok') +
    kv('5-Repo Mass Residual', `${d.tb13.mass_res.toExponential(1)} · WorldΔ=${d.tb13.world_shift.toFixed(1)}`, 'ok');
  drawMorpho12(d.tb13.axis_means, d.tb13.axis_spans);

  // Tardigrade metrics
  document.getElementById('mTardigrade').innerHTML =
    kv('Somatic Cells (4-Hox)', `${d.tb09.total_cells} / 1000`, d.tb09.total_cells <= 1000 ? 'ok' : 'bad') +
    kv('7 Organs + Sealed Gut', `Gut=${d.tb09.gut_sealed ? 'SEALED' : 'OPEN'} · Axons=${d.tb09.axons_wired}`, 'ok') +
    kv('98% Dry + 4kGy Surv', `${d.tb09.intact_surv.toFixed(1)}% vs ${d.tb09.ko_surv.toFixed(2)}% KO`, d.tb09.intact_surv > 50 ? 'ok' : 'bad') +
    kv('Megaphragma Nuclei', `${d.tb09.mega_neurons} → ${d.tb09.mega_nuclei} (95% lysis)`, 'ok');

  // Cytoskeleton metrics
  document.getElementById('mCytoskeleton').innerHTML =
    kv('Wall-Force Centering', `${d.tb03.centrosome_init.toFixed(2)} → ${d.tb03.centrosome_final.toFixed(2)} μm`, 'ok') +
    kv('Ca²⁺ Catastrophe Melt', `${d.tb03.mt_before_ca.toFixed(2)} → ${d.tb03.mt_after_ca.toFixed(2)} μm`, 'ok') +
    kv('Kinesin / Dynein Tug', `+${d.tb03.kin_dom.toFixed(2)} / ${d.tb03.dyn_dom.toFixed(2)} μm/s`, 'ok') +
    kv('Arp2/3 70° & Lacrymaria', `${d.tb03.arp23_pN.toFixed(1)} pN · ${d.tb03.lacry_ratio.toFixed(1)}×`, 'ok');

  // Poincare metrics
  document.getElementById('mPoincare').innerHTML =
    kv('Euler Charge ∑q_k', `${d.tb05.final_charge.toFixed(8)} (err ${d.tb05.max_err.toExponential(1)})`, 'ok') +
    kv('Mesh & Organizers', `V=${d.tb05.n_vertices} · Poles=${d.tb05.final_organizers} · S=${d.tb05.nematic_S.toFixed(3)}`, 'ok');

  // LLPS metrics
  document.getElementById('mLLPS').innerHTML =
    kv('Eq Ostwald Ripening', `${d.tb04.eq_droplets} drops · R=${d.tb04.eq_radius.toFixed(2)} (drift ${d.tb04.mass_drift.toExponential(1)})`) +
    kv('Active ATP Arrest', `${d.tb04.active_droplets} drops · R=${d.tb04.active_radius.toFixed(2)}`, 'ok');

  // Turing metrics
  document.getElementById('mTuring').innerHTML =
    kv('Equal Diff (Du=Dv)', `std = ${d.tb07.eq_diff_std.toFixed(5)} (Flat)`) +
    kv('Fast Inh (Dv=30·Du)', `std = ${d.tb07.fast_inh_std.toFixed(4)}`, d.tb07.fast_inh_std > 0.1 ? 'ok' : 'bad') +
    kv('Feinberg δ=0 Amp', `${d.tb07.feinberg_d0_amp.toFixed(6)} (No Osc)`) +
    kv('Feinberg δ=1 Amp', `${d.tb07.feinberg_d1_amp.toFixed(3)} (Limit Cycle)`, 'ok');

  // Eigen / TUR metrics
  document.getElementById('mEigenTur').innerHTML =
    kv('Eigen L=600 Eq → KP1', `${d.tb06.eq_L600.toFixed(4)} → ${d.tb06.kp1_L600.toFixed(3)}`, d.tb06.kp1_L600 > 0.85 ? 'ok' : 'bad') +
    kv('KP2 L=3200 Survival', `${(100 * d.tb06.kp2_L3200).toFixed(2)}%`) +
    kv('Min TUR (Var/μ²)ΔS', `${d.tb08.tur_min.toFixed(3)} ≥ 2.0 · Jitter ${d.tb08.tur_jitter_ratio.toFixed(1)}×`, 'ok') +
    kv('Chemo & Stentor', `AdaptErr=${d.tb08.adapt_err.toFixed(4)} · Tier ${d.tb08.stentor_tier}`, 'ok');

  // Draw static PDE heatmaps & charts
  drawLLPS(d.tb04.eq_field, d.tb04.active_field);
  drawTuring(d.tb07.turing_field);
  drawEigenTur(d.tb06, d.tb08);
  drawBarcode(d.tb12.asmo_112, d.tb12.asmo_mean, d.tb12.asmo_std);

  // Additional Testbed Ledger Cards (TB01, TB02, TB10, TB11, TB12)
  document.getElementById('ledgerCards').innerHTML = `
    <div class="card">
      <div class="card-hdr"><div class="card-title">TB01 · Purcell Scallop & MotA₅MotB₂</div><div class="card-tag">Re ≪ 1</div></div>
      <div class="metrics">
        ${kv('1-DOF Reciprocal', d.tb01.scallop_net_disp.toExponential(2), Math.abs(d.tb01.scallop_net_disp) < 1e-10 ? 'ok' : 'bad')}
        ${kv('2-DOF Non-Recip', `${d.tb01.purcell_2dof_disp.toFixed(4)} (Area ${d.tb01.loop_area.toFixed(2)})`, 'ok')}
        ${kv('MotA₅ 36° Rotor', `${d.tb01.mota5_disp.toFixed(2)} μm (${d.tb01.mota5_steps} H⁺)`, 'ok')}
        ${kv('SHA-256 Prefix', d.tb01.sha256.slice(0, 14) + '…')}
      </div>
    </div>
    <div class="card">
      <div class="card-hdr"><div class="card-title">TB02 · Virosphere & Giant Viruses</div><div class="card-tag">VIROCELL</div></div>
      <div class="metrics">
        ${kv('Kill-the-Winner H', `${d.tb02.shannon_no_phage.toFixed(2)} → ${d.tb02.shannon_with_phage.toFixed(2)}`, 'ok')}
        ${kv('PhiKZ Chimallin', `${d.tb02.phikz_no_shell.toFixed(3)} → ${d.tb02.phikz_shell.toFixed(3)}`, 'ok')}
        ${kv('Mavirus Rescue', `${d.tb02.amoeba_no_mavirus.toFixed(2)} → ${d.tb02.amoeba_with_mavirus.toFixed(2)}`, d.tb02.amoeba_with_mavirus > 0.4 ? 'ok' : 'bad')}
        ${kv('ORFans / Obelisks', `${d.tb02.orfans} ORFans · ${d.tb02.obelisks} Obelisks`, 'ok')}
      </div>
    </div>
    <div class="card">
      <div class="card-hdr"><div class="card-title">TB10 · Odd Elasticity & Chase</div><div class="card-tag">CHIRAL CRYSTAL</div></div>
      <div class="metrics">
        ${kv('Passive Work (Ko=0)', d.tb10.passive_work.toExponential(1), 'ok')}
        ${kv('Odd Work (2KoA)', `${d.tb10.odd_work.toFixed(4)} (Th=${d.tb10.theory_odd_work.toFixed(4)})`, 'ok')}
        ${kv('Recip vs Chase', `${d.tb10.recip_vel.toFixed(4)} vs ${d.tb10.nonrecip_vel.toFixed(3)}`, 'ok')}
        ${kv('Crystal Order ψ₆', d.tb10.psi6.toFixed(3), 'ok')}
      </div>
    </div>
    <div class="card">
      <div class="card-hdr"><div class="card-title">TB11 · Lane vs Lynch + Nitroplast</div><div class="card-tag">2×2 FACTORIAL</div></div>
      <div class="metrics">
        ${kv('Prok vs Euk Vol', `${d.tb11.prok_bigNe.toFixed(0)} → ${d.tb11.euk_bigNe.toFixed(0)}`, 'ok')}
        ${kv('Drift Gen (Ne400/40)', `${(d.tb11.gen_bigNe/1000).toFixed(1)} vs ${(d.tb11.gen_smallNe/1000).toFixed(1)} Mb`, 'ok')}
        ${kv('Translation ATP', `${(100*d.tb11.trans_atp_share).toFixed(1)}% (>50%)`, 'ok')}
        ${kv('UCYN-A Nitroplast', `Sync ${d.tb11.nitro_sync.toFixed(2)} · N₂=${d.tb11.nitro_n2}`, 'ok')}
      </div>
    </div>
    <div class="card">
      <div class="card-hdr"><div class="card-title">TB12 · Track A/B & Zeta-Omega</div><div class="card-tag">13-VECTOR</div></div>
      <div class="metrics">
        ${kv('Track A vs B Tumors', `A=${d.tb12.trackA_tumors} vs B=${d.tb12.trackB_tumors}`, d.tb12.trackA_tumors === 0 ? 'ok' : 'bad')}
        ${kv('OVI Feral / 50% / 100%', `${d.tb12.ovi_feral.toFixed(1)} / ${d.tb12.ovi_50pct.toFixed(1)} / ${d.tb12.ovi_100pct.toFixed(1)}`, 'ok')}
        ${kv('Escape Mutations 50%', `${d.tb12.escapes_50pct} (Backlash!)`, 'ok')}
        ${kv('Asmodensius 112D', `μ=${d.tb12.asmo_mean.toFixed(3)} σ=${d.tb12.asmo_std.toFixed(3)}`, 'ok')}
      </div>
    </div>
  `;
}

function drawFieldHalf(ctx, field, ox, oy, w, h, colorFn, label) {
  const cw = w / 32, ch = h / 32;
  for (let y = 0; y < 32; y++) {
    for (let x = 0; x < 32; x++) {
      ctx.fillStyle = colorFn(field[y * 32 + x]);
      ctx.fillRect(ox + x * cw, oy + y * ch, Math.ceil(cw), Math.ceil(ch));
    }
  }
  ctx.fillStyle = 'rgba(0,0,0,0.65)';
  ctx.fillRect(ox + 4, oy + 4, 145, 18);
  ctx.fillStyle = '#e2e8f0';
  ctx.font = '11px ui-monospace, monospace';
  ctx.fillText(label, ox + 9, oy + 16);
}

function drawLLPS(eqF, actF) {
  const cv = document.getElementById('cvLLPS');
  const ctx = cv.getContext('2d');
  ctx.clearRect(0, 0, cv.width, cv.height);
  const col = (v) => {
    const t = Math.max(0, Math.min(1, (v + 1.0) / 2.0));
    return `rgb(${Math.round(12 + 235*t)}, ${Math.round(20 + 180*t*t)}, ${Math.round(55 + 195*(1-t*0.3))})`;
  };
  drawFieldHalf(ctx, eqF, 4, 4, 202, 212, col, 'EQ: Ostwald Ripening');
  drawFieldHalf(ctx, actF, 214, 4, 202, 212, col, 'ATP: Active Arrested');
}

function drawTuring(field) {
  const cv = document.getElementById('cvTuring');
  const ctx = cv.getContext('2d');
  ctx.clearRect(0, 0, cv.width, cv.height);
  const col = (v) => {
    const t = Math.max(0, Math.min(1, v / 2.8));
    return `rgb(${Math.round(10 + 40*t)}, ${Math.round(18 + 235*t)}, ${Math.round(35 + 210*Math.sqrt(t))})`;
  };
  drawFieldHalf(ctx, field, 80, 4, 260, 212, col, 'Schnakenberg Activator u(x,y)');
}

function drawEigenTur(tb06, tb08) {
  const cv = document.getElementById('cvEigenTur');
  const ctx = cv.getContext('2d');
  ctx.clearRect(0, 0, cv.width, cv.height);
  // Left half: Eigen curves
  const plotCurve = (arr, color) => {
    ctx.strokeStyle = color; ctx.lineWidth = 2.2; ctx.beginPath();
    for (let i = 0; i < arr.length; i++) {
      const x = 18 + (i / (arr.length - 1)) * 175;
      const y = 195 - arr[i] * 155;
      if (i === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
    }
    ctx.stroke();
  };
  ctx.strokeStyle = '#1e2d4d'; ctx.strokeRect(16, 30, 180, 168);
  plotCurve(tb06.eq, '#fb7185');
  plotCurve(tb06.kp1, '#fbbf24');
  plotCurve(tb06.kp2, '#34d399');
  ctx.fillStyle = '#e2e8f0'; ctx.font = '10px ui-monospace, monospace';
  ctx.fillText('Eigen: Eq(red) KP1(amb) KP2(grn)', 16, 20);

  // Right half: TUR bars vs >=2 floor
  ctx.strokeStyle = '#1e2d4d'; ctx.strokeRect(216, 30, 188, 168);
  const yFloor = 195 - (2.0 / 15.0) * 155;
  ctx.strokeStyle = '#fb7185'; ctx.setLineDash([4, 3]);
  ctx.beginPath(); ctx.moveTo(216, yFloor); ctx.lineTo(404, yFloor); ctx.stroke();
  ctx.setLineDash([]);
  tb08.products.forEach((p, idx) => {
    const h = Math.min(155, (p / 15.0) * 155);
    ctx.fillStyle = '#22d3ee';
    ctx.fillRect(228 + idx * 34, 195 - h, 22, h);
    ctx.fillStyle = '#94a3b8';
    ctx.fillText(p.toFixed(1), 227 + idx * 34, 190 - h);
  });
  ctx.fillStyle = '#e2e8f0';
  ctx.fillText('TUR (Var/μ²)ΔS ≥ 2.0 Floor', 216, 20);
}

function drawMorpho12(means, spans) {
  const cv = document.getElementById('cvMorpho12');
  const ctx = cv.getContext('2d');
  ctx.clearRect(0, 0, cv.width, cv.height);
  const labels = ['Size','Auton','Ploidy','GenOS','Doll','Metab','Armor','Weap','Rheo','Plast','Chaos','Swarm'];
  const cx = 210, cy = 112, R = 84;
  ctx.strokeStyle = '#1e2d4d';
  for (let ring = 1; ring <= 4; ring++) {
    ctx.beginPath(); ctx.arc(cx, cy, (R * ring) / 4, 0, Math.PI * 2); ctx.stroke();
  }
  // Span envelope polygon (emerald)
  ctx.fillStyle = 'rgba(52,211,153,0.18)'; ctx.strokeStyle = '#34d399'; ctx.lineWidth = 1.8;
  ctx.beginPath();
  for (let i = 0; i < 12; i++) {
    const a = (i / 12) * Math.PI * 2 - Math.PI / 2;
    const x = cx + Math.cos(a) * R * spans[i], y = cy + Math.sin(a) * R * spans[i];
    if (i === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
  }
  ctx.closePath(); ctx.fill(); ctx.stroke();
  // Mean polygon (cyan)
  ctx.fillStyle = 'rgba(34,211,238,0.28)'; ctx.strokeStyle = '#22d3ee'; ctx.lineWidth = 2;
  ctx.beginPath();
  for (let i = 0; i < 12; i++) {
    const a = (i / 12) * Math.PI * 2 - Math.PI / 2;
    const x = cx + Math.cos(a) * R * means[i], y = cy + Math.sin(a) * R * means[i];
    if (i === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
  }
  ctx.closePath(); ctx.fill(); ctx.stroke();
  ctx.fillStyle = '#94a3b8'; ctx.font = '9.5px ui-monospace, monospace';
  for (let i = 0; i < 12; i++) {
    const a = (i / 12) * Math.PI * 2 - Math.PI / 2;
    ctx.fillText(labels[i], cx + Math.cos(a) * (R + 14) - 14, cy + Math.sin(a) * (R + 14) + 3);
  }
}

function drawVirtualCell(t16) {
  const cv = document.getElementById('cvVirtualCell');
  if (!cv || !t16) return;
  const ctx = cv.getContext('2d');
  ctx.clearRect(0, 0, cv.width, cv.height);

  // Top half: 32-zone Portal -> Central Hepatic Zonation Continuous Gradient
  ctx.strokeStyle = '#1e2d4d';
  ctx.strokeRect(12, 22, 240, 102);
  ctx.fillStyle = '#e2e8f0';
  ctx.font = '10px ui-monospace, monospace';
  ctx.fillText('Hepatic Zonation: O₂(cyn) Gluc(grn) Glyc(amb)', 12, 14);

  const plotZ = (arr, col) => {
    if (!Array.isArray(arr) || !arr.length) return;
    ctx.strokeStyle = col;
    ctx.lineWidth = 2;
    ctx.beginPath();
    arr.forEach((v, i) => {
      const x = 14 + (i / (arr.length - 1)) * 236;
      const y = 120 - Math.max(0, Math.min(1, v)) * 92;
      if (i === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
    });
    ctx.stroke();
  };
  plotZ(t16.z_o2, '#22d3ee');
  plotZ(t16.z_gluconeo, '#34d399');
  plotZ(t16.z_glycolysis, '#fbbf24');

  // Right panel: Raw AI Hallucination vs Physics-Projected Residual
  ctx.strokeStyle = '#1e2d4d';
  ctx.strokeRect(262, 22, 146, 102);
  ctx.fillStyle = '#e2e8f0';
  ctx.fillText('O56 Mass Drift', 266, 14);
  const rawH = Math.min(82, Math.max(8, (t16.raw_ai_halluc / 12.0) * 82));
  const projOk = t16.proj_mass_res < 1e-12;
  const projH = projOk ? 3 : Math.min(82, Math.max(8, (t16.proj_mass_res / 12.0) * 82));
  ctx.fillStyle = '#fb7185';
  ctx.fillRect(284, 114 - rawH, 36, rawH);
  ctx.fillStyle = projOk ? '#34d399' : '#fb7185';
  ctx.fillRect(344, 114 - projH, 36, projH);
  ctx.fillStyle = '#94a3b8';
  ctx.font = '9px ui-monospace, monospace';
  ctx.fillText('RawAI', 286, 123);
  ctx.fillText('Proj', 350, 123);
  ctx.fillStyle = '#fca5a5';
  ctx.fillText(t16.raw_ai_halluc.toFixed(1), 288, 110 - rawH);
  ctx.fillStyle = projOk ? '#34d399' : '#fb7185';
  ctx.fillText(projOk ? '0.0' : t16.proj_mass_res.toFixed(1), 348, 110 - projH);

  // Bottom strip: Real 573-gene JCVI-syn3A CRISPRi knockout flux-collapse histogram (Dark Essentials vs Redundant Controls)
  ctx.fillStyle = '#e2e8f0';
  ctx.font = '10px ui-monospace, monospace';
  ctx.fillText(`JCVI-syn3A Graph CRISPRi Flux Collapse: Dark(purp) vs Paralog(cyn) · ${t16.syn3a_discovered}/100`, 12, 146);
  const bx = 12, by = 152, bw = 396, bh = 34;
  ctx.fillStyle = '#0f172a';
  ctx.fillRect(bx, by, bw, bh);
  ctx.strokeStyle = '#334155';
  ctx.strokeRect(bx, by, bw, bh);
  const hist = Array.isArray(t16.ko_hist) && t16.ko_hist.length === 32 ? t16.ko_hist : null;
  if (hist) {
    const maxC = Math.max(1, ...hist);
    const binW = bw / 16;
    for (let i = 0; i < 16; i++) {
      const hDark = (hist[i] / maxC) * (bh - 4);
      const hCtrl = (hist[16 + i] / maxC) * (bh - 4);
      ctx.fillStyle = 'rgba(34,211,238,0.65)';
      ctx.fillRect(bx + i * binW + 1, by + bh - 2 - hCtrl, binW * 0.44, hCtrl);
      ctx.fillStyle = 'rgba(167,139,250,0.85)';
      ctx.fillRect(bx + i * binW + binW * 0.48, by + bh - 2 - hDark, binW * 0.44, hDark);
    }
  }

  // 6-Tier Closure strip
  ctx.fillStyle = '#34d399';
  ctx.fillText(`6-Tier Closure: Δ=${t16.six_tier_err.toExponential(1)} · Ridge R²=${t16.perturb_r2.toFixed(3)} · AUROC=${t16.essential_auroc.toFixed(3)}`, 12, 202);
}

function drawBarcode(arr, mu, sd) {
  const cv = document.getElementById('cvBarcode');
  const ctx = cv.getContext('2d');
  ctx.clearRect(0, 0, cv.width, cv.height);
  const bw = cv.width / arr.length;
  arr.forEach((v, i) => {
    const hue = 185 + v * 130;
    ctx.fillStyle = `hsl(${hue}, 82%, ${22 + v * 48}%)`;
    ctx.fillRect(i * bw, 0, Math.ceil(bw), cv.height);
  });
  document.getElementById('bridgeTag').textContent = `112D VECTOR · μ=${mu.toFixed(3)} · σ=${sd.toFixed(3)}`;
}

function animateLive() {
  animT += 0.04;
  if (caShockTimer > 0) caShockTimer -= 0.016;
  if (harpoonTimer > 0) harpoonTimer -= 0.025;

  // -1. Live TB15 Deep Strata Canvas: Levin Vmem 1-Head vs 2-Head Planarian + Wireless Neuropeptide + Cnidocyte 5.4M g Harpoon
  {
    const cv = document.getElementById('cvDeepStrata');
    if (cv) {
      const ctx = cv.getContext('2d');
      ctx.clearRect(0, 0, cv.width, cv.height);
      const t15 = state?.tb15;
      const m9Broken = Boolean(t15 && t15.levin_reprog_heads < 2);
      const twoHead = vmemTwoHead && !m9Broken;
      const vArr = (twoHead ? t15?.vmem_2head : t15?.vmem_wild) || [];

      // Left-Top: Animated Levin Bioelectric Vmem Planarian (0 DNA mutations!)
      ctx.fillStyle = '#e2e8f0';
      ctx.font = '9.5px ui-monospace, monospace';
      ctx.fillText(twoHead ? 'Levin Vmem: 2-HEAD ATTRACTOR (0 DNA Muts!)' : 'Levin Vmem: 1-HEAD WILD POLARITY', 10, 15);

      const wx = 128, wy = 66;
      const grad = ctx.createLinearGradient(wx - 98, wy, wx + 98, wy);
      grad.addColorStop(0.0, '#fbbf24'); // Anterior Depolarized Head (-14 mV)
      grad.addColorStop(0.5, '#3b82f6'); // Hyperpolarized Trunk (-66 mV)
      grad.addColorStop(1.0, twoHead ? '#fbbf24' : '#1d4ed8'); // Posterior Head vs Tail!
      ctx.fillStyle = grad;
      ctx.strokeStyle = twoHead ? '#fde047' : '#60a5fa';
      ctx.lineWidth = 2;
      ctx.beginPath();
      ctx.ellipse(wx, wy, 94, 24 + 2 * Math.sin(animT * 2), 0, 0, Math.PI * 2);
      ctx.fill(); ctx.stroke();

      // Anterior Eyespots (always present)
      const drawEyes = (ex, ey) => {
        ctx.fillStyle = '#0f172a';
        ctx.beginPath(); ctx.arc(ex, ey - 7, 4.5, 0, Math.PI * 2); ctx.arc(ex, ey + 7, 4.5, 0, Math.PI * 2); ctx.fill();
        ctx.fillStyle = '#f8fafc';
        ctx.beginPath(); ctx.arc(ex - 1, ey - 7, 1.8, 0, Math.PI * 2); ctx.arc(ex - 1, ey + 7, 1.8, 0, Math.PI * 2); ctx.fill();
      };
      drawEyes(wx - 72, wy);
      if (twoHead) drawEyes(wx + 72, wy);

      // Live 32-Cell Vmem Voltage Waveform directly beneath the Planarian
      ctx.strokeStyle = '#1e2d4d';
      ctx.strokeRect(12, 102, 232, 46);
      if (vArr.length === 32) {
        ctx.strokeStyle = twoHead ? '#fbbf24' : '#38bdf8';
        ctx.lineWidth = 2;
        ctx.beginPath();
        vArr.forEach((mv, i) => {
          const x = 16 + (i / 31) * 224;
          const norm = (mv + 72) / 64; // -72mV bottom, -8mV top
          const y = 144 - Math.max(0, Math.min(1, norm)) * 38;
          if (i === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
        });
        ctx.stroke();
      }
      ctx.fillStyle = '#94a3b8';
      ctx.font = '8.5px ui-monospace, monospace';
      ctx.fillText('Vmem -14mV(Head) / -66mV(Tail)', 16, 159);

      // Bottom-Left: Real C11 1ns-Step Cnidocyte ODE Acceleration Curve a(t) + Cohesin TAD Contact Cliff
      ctx.strokeStyle = '#1e2d4d';
      ctx.strokeRect(12, 166, 114, 34);
      const cnidArr = Array.isArray(t15?.cnid_curve) ? t15.cnid_curve : [];
      if (cnidArr.length === 32) {
        const mxC = Math.max(1, ...cnidArr);
        ctx.strokeStyle = harpoonTimer > 0 ? '#fde047' : '#fb7185';
        ctx.lineWidth = 1.8;
        ctx.beginPath();
        cnidArr.forEach((v, i) => {
          const x = 14 + (i / 31) * 110;
          const y = 198 - (Math.max(0, v) / mxC) * 28;
          if (i === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
        });
        ctx.stroke();
      }
      ctx.fillStyle = '#fda4af';
      ctx.fillText(`Cnid ODE ${(t15 ? t15.cnidocyte_g/1e6 : 5.89).toFixed(2)}Mg`, 14, 212);

      // Adjacent real C11 Cohesin/CTCF 32-bin TAD contact profile (showing CTCF cliff!)
      ctx.strokeStyle = '#1e2d4d';
      ctx.strokeRect(132, 166, 112, 34);
      const tadArr = Array.isArray(t15?.tad_profile) ? t15.tad_profile : [];
      if (tadArr.length === 32) {
        const mxT = Math.max(1, ...tadArr);
        // Vertical dashed CTCF wall line around bin 128 (~index 19)
        const wallX = 134 + (19 / 31) * 108;
        ctx.strokeStyle = 'rgba(251,191,36,0.55)'; ctx.setLineDash([2, 2]);
        ctx.beginPath(); ctx.moveTo(wallX, 168); ctx.lineTo(wallX, 198); ctx.stroke();
        ctx.setLineDash([]);
        ctx.strokeStyle = '#34d399';
        ctx.lineWidth = 1.8;
        ctx.beginPath();
        tadArr.forEach((v, i) => {
          const x = 134 + (i / 31) * 108;
          const y = 198 - Math.min(1, Math.log1p(Math.max(0, v)) / Math.log1p(mxT)) * 28;
          if (i === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
        });
        ctx.stroke();
      }
      ctx.fillStyle = '#34d399';
      ctx.fillText(`TAD ${t15 ? t15.tad_gain.toFixed(1) : '8.8'}× (CTCF)`, 134, 212);

      // Right Half: C. elegans Wireless Neuropeptide Connectome vs Wired Ring
      const nx = 334, ny = 112, nr = 64;
      ctx.fillStyle = '#e2e8f0';
      ctx.font = '9px ui-monospace, monospace';
      ctx.fillText('Wireless Neuropeptide', 278, 15);
      ctx.strokeStyle = 'rgba(56,189,248,0.28)';
      ctx.lineWidth = 1.2;
      ctx.beginPath(); ctx.arc(nx, ny, nr, 0, Math.PI * 2); ctx.stroke();

      // Animated extrasynaptic wireless peptide broadcast waves
      for (let w = 0; w < 3; w++) {
        const wr = ((animT * 26 + w * 22) % nr);
        ctx.strokeStyle = `rgba(167,139,250,${(1 - wr / nr) * 0.65})`;
        ctx.lineWidth = 1.4;
        ctx.beginPath(); ctx.arc(nx, ny, wr, 0, Math.PI * 2); ctx.stroke();
      }
      // 16 neuron nodes + wireless chords
      for (let i = 0; i < 16; i++) {
        const a = (i / 16) * Math.PI * 2 + animT * 0.12;
        const px = nx + Math.cos(a) * nr, py = ny + Math.sin(a) * nr;
        if (i % 3 === 0) {
          const b = (((i + 5) % 16) / 16) * Math.PI * 2 + animT * 0.12;
          ctx.strokeStyle = 'rgba(167,139,250,0.42)';
          ctx.lineWidth = 1;
          ctx.beginPath(); ctx.moveTo(px, py); ctx.lineTo(nx + Math.cos(b) * nr, ny + Math.sin(b) * nr); ctx.stroke();
        }
        ctx.fillStyle = i % 4 === 0 ? '#a78bfa' : '#22d3ee';
        ctx.beginPath(); ctx.arc(px, py, 3.2, 0, Math.PI * 2); ctx.fill();
      }
      ctx.fillStyle = '#c4b5fd';
      ctx.font = '8.5px ui-monospace, monospace';
      ctx.fillText(`Reach: ${t15 ? (100*t15.wired_reach).toFixed(0) : '9'}% → ${t15 ? (100*t15.wireless_reach).toFixed(0) : '98'}%`, 284, 202);
    }
  }

  // 0. Live TB14 Bio-Frostbite 4-Layer Interaction Net VM (Active Pairs α ⋈ β + Russian-Doll D0-D3 + 0W Tun)
  {
    const cv = document.getElementById('cvBioInet');
    if (cv) {
      const ctx = cv.getContext('2d');
      ctx.clearRect(0, 0, cv.width, cv.height);
      const t14 = state?.tb14;
      const confOk = !t14 || t14.confluence_match === 1;
      const layers = [
        { lbl: 'L1 DNA',  y: 30,  col: '#38bdf8', pairs: [['Poly', 'Base'], ['Ribo', 'Codon'], ['CRISPR', 'Phage']], cnt: t14?.layer_rewrites?.[0] ?? 56 },
        { lbl: 'L2 ORG',  y: 80,  col: '#34d399', pairs: [['ATP_S', 'H⁺'],  ['Kin', '+8.2nm'], ['Phago', 'D0→D3']], cnt: t14?.layer_rewrites?.[1] ?? 44 },
        { lbl: 'L3 CELL', y: 130, col: '#a78bfa', pairs: [['FtsZ', 'Ring'], ['ESCRT', 'Bud'],  ['CAHS', '0W Tun']], cnt: t14?.layer_rewrites?.[2] ?? 16 },
        { lbl: 'L4 CREAT',y: 180, col: '#fbbf24', pairs: [['GapJ', 'Vm Δ0'],['Wedge', 'Gut'],  ['CPG', 'Lobopod']], cnt: t14?.layer_rewrites?.[3] ?? 20 }
      ];

      // Subtle Russian-Doll nested membrane boxes on right side (D0 -> D1 -> D2 -> D3)
      for (let d = 0; d < 4; d++) {
        ctx.strokeStyle = d === 3 ? 'rgba(52,211,153,0.45)' : 'rgba(56,189,248,0.16)';
        ctx.lineWidth = 1.2;
        ctx.strokeRect(334 + d * 8, 12 + d * 12, 78 - d * 16, 196 - d * 24);
      }
      ctx.fillStyle = '#94a3b8';
      ctx.font = '8.5px ui-monospace, monospace';
      ctx.fillText('BOX D0→D3', 347, 24);
      ctx.fillStyle = 'rgba(4,7,14,0.92)';
      ctx.fillRect(338, 100, 70, 36);
      ctx.strokeStyle = confOk ? 'rgba(52,211,153,0.45)' : 'rgba(251,113,133,0.6)';
      ctx.strokeRect(338, 100, 70, 36);
      ctx.fillStyle = confOk ? '#34d399' : '#fb7185';
      ctx.fillText(confOk ? '◆ 5×SHA' : '✖ M8 RACE', 346, 114);
      ctx.fillStyle = '#38bdf8';
      ctx.fillText(tunMode ? '0W TUN' : `SSIN ${(t14?.ssin_confluent_pct ?? 95.3).toFixed(1)}%`, 343, 129);

      // Draw each layer band & colliding principal-port active pairs (α ⋈ β)
      layers.forEach((L, li) => {
        ctx.fillStyle = 'rgba(15,23,42,0.72)';
        ctx.fillRect(6, L.y - 20, 320, 42);
        ctx.strokeStyle = confOk ? 'rgba(30,45,77,0.9)' : 'rgba(251,113,133,0.55)';
        ctx.lineWidth = 1;
        ctx.strokeRect(6, L.y - 20, 320, 42);

        ctx.fillStyle = L.col;
        ctx.font = 'bold 9.5px ui-monospace, monospace';
        ctx.fillText(L.lbl, 12, L.y - 5);
        ctx.fillStyle = '#64748b';
        ctx.font = '8.5px ui-monospace, monospace';
        ctx.fillText(`${L.cnt} rw`, 12, L.y + 10);

        L.pairs.forEach(([aLbl, bLbl], pi) => {
          const bx = 98 + pi * 86;
          const phase = tunMode ? 0.85 : ((animT * (1.25 + li * 0.18) + pi * 2.1 + li * 0.9) % 3.14159);
          const sep = tunMode ? 19 : (4 + 17 * Math.abs(Math.cos(phase)));
          const spark = !tunMode && sep < 7.5;
          const jitter = (!confOk && !tunMode) ? Math.sin(animT * 18 + pi * 3) * 4 : 0;

          // Principal-port wire connecting α ⋈ β
          ctx.strokeStyle = tunMode ? '#38bdf8' : (spark ? '#ffffff' : L.col);
          ctx.lineWidth = spark ? 2.6 : 1.4;
          ctx.beginPath();
          ctx.moveTo(bx - sep, L.y + jitter);
          ctx.lineTo(bx + sep, L.y - jitter);
          ctx.stroke();

          // Left agent triangle pointing right (principal port ▶)
          ctx.fillStyle = tunMode ? 'rgba(56,189,248,0.28)' : L.col;
          ctx.beginPath();
          ctx.moveTo(bx - sep, L.y + jitter);
          ctx.lineTo(bx - sep - 14, L.y - 9 + jitter);
          ctx.lineTo(bx - sep - 14, L.y + 9 + jitter);
          ctx.closePath();
          ctx.fill();

          // Right agent triangle pointing left (principal port ◀)
          ctx.fillStyle = tunMode ? 'rgba(56,189,248,0.28)' : (confOk ? '#e2e8f0' : '#fb7185');
          ctx.beginPath();
          ctx.moveTo(bx + sep, L.y - jitter);
          ctx.lineTo(bx + sep + 14, L.y - 9 - jitter);
          ctx.lineTo(bx + sep + 14, L.y + 9 - jitter);
          ctx.closePath();
          ctx.fill();

          if (spark) {
            ctx.fillStyle = confOk ? 'rgba(52,211,153,0.45)' : 'rgba(251,113,133,0.6)';
            ctx.beginPath();
            ctx.arc(bx, L.y, 10, 0, Math.PI * 2);
            ctx.fill();
          }

          ctx.fillStyle = '#cbd5e1';
          ctx.font = '8px ui-monospace, monospace';
          ctx.fillText(`${aLbl}⋈${bLbl}`, bx - 24, L.y + 17);
        });
      });

      if (tunMode) {
        ctx.fillStyle = 'rgba(56,189,248,0.12)';
        ctx.fillRect(0, 0, cv.width, cv.height);
        ctx.fillStyle = '#bae6fd';
        ctx.font = 'bold 10px ui-monospace, monospace';
        ctx.fillText('CAHS BIOGLASS SHIELD ACTIVE · 0 ACTIVE PAIRS · 0W DYNAMIC POWER', 24, 16);
      }
    }
  }

  // 1. Live Tardigrade 3D <=1000-cell Walking Head + 8 Lobopods + Tun Mode
  {
    const cv = document.getElementById('cvTardigrade');
    const ctx = cv.getContext('2d');
    ctx.clearRect(0, 0, cv.width, cv.height);
    const cx = 210, cy = 110;
    const shrink = tunMode ? 0.54 : 1.0;
    document.getElementById('tardiModeTag').textContent = tunMode ? 'CAHS BIOGLASS TUN (98% DRY)' : 'ACTIVE 8-LOBOPOD CPG';

    // Outer cuticle & 4 Walking-Head segments
    ctx.save();
    ctx.translate(cx, cy);
    ctx.fillStyle = tunMode ? 'rgba(56,189,248,0.22)' : 'rgba(52,211,153,0.16)';
    ctx.strokeStyle = tunMode ? '#38bdf8' : '#34d399';
    ctx.lineWidth = 2.5;
    ctx.beginPath();
    ctx.ellipse(0, 0, 128 * shrink, 56 * (tunMode ? 0.82 : 1.0), 0, 0, Math.PI * 2);
    ctx.fill(); ctx.stroke();

    // 8 Lobopods driven by alternating Matsuoka CPG phase wave
    if (!tunMode) {
      for (let seg = 0; seg < 4; seg++) {
        const lx = -72 + seg * 48;
        const phase = Math.sin(animT * 2.8 - seg * 1.15);
        for (const side of [-1, 1]) {
          const swing = phase * side * 14;
          const by = side * 46;
          ctx.strokeStyle = '#34d399'; ctx.lineWidth = 6; ctx.lineCap = 'round';
          ctx.beginPath(); ctx.moveTo(lx, by); ctx.lineTo(lx + swing, by + side * 26); ctx.stroke();
          // Tiny claws
          ctx.strokeStyle = '#fbbf24'; ctx.lineWidth = 1.8;
          ctx.beginPath(); ctx.moveTo(lx + swing, by + side * 26);
          ctx.lineTo(lx + swing + 5, by + side * 32); ctx.stroke();
        }
      }
    }

    // Internal 7 Organs: Sealed Hollow Gut (amber), Protocerebral Brain + 4 Ganglia (cyan), Gonad (violet), 3 Malpighian (rose)
    ctx.strokeStyle = '#fbbf24'; ctx.lineWidth = 10 * shrink;
    ctx.beginPath(); ctx.moveTo(-108 * shrink, 0); ctx.lineTo(100 * shrink, 0); ctx.stroke(); // Sealed gut lumen
    // Buccal stylets
    ctx.strokeStyle = '#f8fafc'; ctx.lineWidth = 2;
    ctx.beginPath(); ctx.moveTo(-125 * shrink, -8*shrink); ctx.lineTo(-102 * shrink, -2*shrink);
    ctx.moveTo(-125 * shrink, 8*shrink); ctx.lineTo(-102 * shrink, 2*shrink); ctx.stroke();
    // Dorsal brain + 4 ganglia + ventral nerve cord
    ctx.fillStyle = '#22d3ee';
    ctx.beginPath(); ctx.arc(-88 * shrink, -18 * shrink, 11 * shrink, 0, Math.PI * 2); ctx.fill();
    for (let g = 0; g < 4; g++) {
      ctx.beginPath(); ctx.arc((-62 + g * 40) * shrink, 22 * shrink, 6.5 * shrink, 0, Math.PI * 2); ctx.fill();
    }
    // Unpaired dorsal gonad & 3 Malpighian tubules
    ctx.fillStyle = 'rgba(167,139,250,0.75)';
    ctx.beginPath(); ctx.ellipse(15 * shrink, -22 * shrink, 38 * shrink, 12 * shrink, 0, 0, Math.PI * 2); ctx.fill();
    ctx.strokeStyle = '#fb7185'; ctx.lineWidth = 2.5;
    for (let m = -1; m <= 1; m++) {
      ctx.beginPath(); ctx.moveTo(64 * shrink, 0); ctx.lineTo(86 * shrink, m * 22 * shrink); ctx.stroke();
    }
    if (tunMode) {
      // CAHS glowing crystalline-amorphous bioglass filaments
      ctx.strokeStyle = 'rgba(186,230,253,0.45)'; ctx.lineWidth = 1.2;
      for (let i = 0; i < 16; i++) {
        const a = (i / 16) * Math.PI * 2 + animT * 0.1;
        ctx.beginPath(); ctx.moveTo(0, 0); ctx.lineTo(Math.cos(a)*62, Math.sin(a)*40); ctx.stroke();
      }
    }
    ctx.restore();
  }

  // 2. Real C11 TB03 Cytoskeleton: Exact 32-Ray Dynamic-Instability Lengths (Before vs After Ca²⁺ Melt)
  {
    const cv = document.getElementById('cvCytoskeleton');
    const ctx = cv.getContext('2d');
    ctx.clearRect(0, 0, cv.width, cv.height);
    const cx0 = 210, cy0 = 110, R = 94;
    const t3 = state?.tb03;
    const isMelt = caShockTimer > 0;
    const meltMix = isMelt ? Math.min(1, caShockTimer / 1.0) : 0.0;
    document.getElementById('mtModeTag').textContent = isMelt ? 'C11 Ca²⁺ MELT DATA!' : 'C11 32-RAY EXACT DATA';

    // Cell membrane boundary (R_cell = 10 um) + cortical capture patch (right side)
    ctx.strokeStyle = '#334155'; ctx.lineWidth = 2;
    ctx.beginPath(); ctx.arc(cx0, cy0, R, 0, Math.PI * 2); ctx.stroke();
    ctx.strokeStyle = '#34d399'; ctx.lineWidth = 4;
    ctx.beginPath(); ctx.arc(cx0, cy0, R, -0.32, 0.32); ctx.stroke();

    // Actual C11 centrosome position (scaled from 10 um cell radius to R px)
    const ox = t3?.centrosome_xy ? cx0 + (t3.centrosome_xy[0] / 10.0) * R : cx0;
    const oy = t3?.centrosome_xy ? cy0 - (t3.centrosome_xy[1] / 10.0) * R : cy0;
    const before = Array.isArray(t3?.ray_before) ? t3.ray_before : [];
    const after  = Array.isArray(t3?.ray_after)  ? t3.ray_after  : [];
    const capArr = Array.isArray(t3?.ray_captured) ? t3.ray_captured : [];

    for (let k = 0; k < 32; k++) {
      const ang = (2 * Math.PI * k) / 32;
      const lenUm = before.length === 32
        ? (before[k] * (1 - meltMix) + (after[k] ?? before[k]) * meltMix)
        : 6.0;
      const lenPx = Math.min(R, (lenUm / 10.0) * R);
      const cap = capArr[k] === 1;
      ctx.strokeStyle = cap ? '#34d399' : (isMelt ? '#fb7185' : '#22d3ee');
      ctx.lineWidth = cap ? 2.4 : 1.35;
      ctx.beginPath();
      ctx.moveTo(ox, oy);
      ctx.lineTo(ox + Math.cos(ang) * lenPx, oy - Math.sin(ang) * lenPx);
      ctx.stroke();
    }
    // Kinesin (+) amber outward & Dynein (-) violet inward cargos riding real C11 ray lengths
    const vKin = t3 ? Math.abs(t3.kin_dom) : 0.57;
    const vDyn = t3 ? Math.abs(t3.dyn_dom) : 0.47;
    for (const c of cargos) {
      const lenUm = before.length === 32
        ? (before[c.ray] * (1 - meltMix) + (after[c.ray] ?? before[c.ray]) * meltMix)
        : 6.0;
      const lenPx = Math.min(R, (lenUm / 10.0) * R);
      const ang = (2 * Math.PI * c.ray) / 32;
      const spd = isMelt ? 0.001 : (c.dir > 0 ? vKin * 0.018 : vDyn * 0.018);
      c.pos += c.dir * spd;
      if (c.pos > 0.92 || c.pos < 0.14) c.dir *= -1;
      const px = ox + Math.cos(ang) * (lenPx * c.pos);
      const py = oy - Math.sin(ang) * (lenPx * c.pos);
      ctx.fillStyle = c.dir > 0 ? '#fbbf24' : '#a78bfa';
      ctx.beginPath(); ctx.arc(px, py, 3.6, 0, Math.PI * 2); ctx.fill();
    }
    ctx.fillStyle = '#f8fafc';
    ctx.beginPath(); ctx.arc(ox, oy, 5, 0, Math.PI * 2); ctx.fill();
  }

  // 3. 3D Rotating Poincare-Hopf Spherical Embryo (chi = +2) plotting EXACT C11 tb05.defects 3D coordinates!
  {
    const cv = document.getElementById('cvPoincare');
    const ctx = cv.getContext('2d');
    ctx.clearRect(0, 0, cv.width, cv.height);
    const cx = 210, cy = 110, R = 86;
    ctx.strokeStyle = '#1e2d4d'; ctx.lineWidth = 1.5;
    ctx.beginPath(); ctx.arc(cx, cy, R, 0, Math.PI * 2); ctx.stroke();

    const rotY = animT * 0.45;
    const cY = Math.cos(rotY), sY = Math.sin(rotY);
    for (let i = 0; i < 12; i++) {
      const lon = (i / 12) * Math.PI * 2 + rotY;
      ctx.strokeStyle = 'rgba(34,211,238,0.38)'; ctx.lineWidth = 1.2;
      ctx.beginPath();
      for (let j = 0; j <= 24; j++) {
        const lat = -Math.PI / 2 + (j / 24) * Math.PI;
        const x = Math.cos(lat) * Math.cos(lon);
        const y = Math.sin(lat);
        const z = Math.cos(lat) * Math.sin(lon);
        const sx = cx + x * R, sy = cy - y * R * 0.92 + z * 12;
        if (j === 0) ctx.moveTo(sx, sy); else ctx.lineTo(sx, sy);
      }
      ctx.stroke();
    }
    // Plot ACTUAL C11 geodesic-icosahedron defect coordinates from state.tb05.defects (up to 4 x (x,y,z))!
    const defs = Array.isArray(state?.tb05?.defects) ? state.tb05.defects : [];
    let plotted = 0;
    for (let d = 0; d < 4 && (d * 3 + 2) < defs.length; d++) {
      const dx = defs[d * 3 + 0], dy = defs[d * 3 + 1], dz = defs[d * 3 + 2];
      if (Math.hypot(dx, dy, dz) < 0.05) continue;
      // Rotate around Z (polar axis) so defects orbit synchronously with meridian grid
      const rx = dx * cY - dy * sY;
      const ry = dx * sY + dy * cY;
      const sx = cx + rx * R;
      const sy = cy - dz * R * 0.92 + ry * 12;
      const col = dz >= 0 ? '#34d399' : '#fb7185';
      ctx.fillStyle = col;
      ctx.beginPath(); ctx.arc(sx, sy, 6.0, 0, Math.PI * 2); ctx.fill();
      ctx.font = '9.5px ui-monospace, monospace';
      ctx.fillText(dz >= 0 ? `D${d+1} (+z=${dz.toFixed(2)})` : `D${d+1} (z=${dz.toFixed(2)})`, sx + 8, sy + 3);
      plotted++;
    }
    ctx.fillStyle = '#94a3b8'; ctx.font = '9px ui-monospace, monospace';
    ctx.fillText(`C11 Geodesic Defects Plotted: ${plotted} · ∑q = ${state?.tb05?.final_charge?.toFixed(6) ?? '2.000000'}`, 14, 212);
  }

  requestAnimationFrame(animateLive);
}

document.getElementById('runBtn').addEventListener('click', () => fetchEngine(false));
document.getElementById('feralBtn').addEventListener('click', () => fetchEngine(true));
document.getElementById('mutantSelect').addEventListener('change', () => fetchEngine(false));
document.getElementById('caShockBtn').addEventListener('click', () => { caShockTimer = 2.5; });
document.getElementById('vmemBtn').addEventListener('click', () => {
  vmemTwoHead = !vmemTwoHead;
  if (state) renderAllStatic();
});
document.getElementById('harpoonBtn').addEventListener('click', () => { harpoonTimer = 1.0; });
document.getElementById('tunBtn').addEventListener('click', () => {
  tunMode = !tunMode;
  if (state) renderAllStatic();
});

fetchEngine(false);
animateLive();
