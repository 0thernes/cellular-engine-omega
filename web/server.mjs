import http from 'node:http';
import fs from 'node:fs';
import path from 'node:path';
import { execFile } from 'node:child_process';
import { fileURLToPath } from 'node:url';

const __dirname = path.dirname(fileURLToPath(import.meta.url));
const ROOT = path.resolve(__dirname, '..');
const BIN = path.join(ROOT, 'bin', process.platform === 'win32' ? 'omega_cell.exe' : 'omega_cell');
const PORT = Number(process.env.PORT || 8791);

const MIME = {
  '.html': 'text/html; charset=utf-8',
  '.css': 'text/css; charset=utf-8',
  '.js': 'application/javascript; charset=utf-8',
  '.json': 'application/json; charset=utf-8',
  '.txt': 'text/plain; charset=utf-8',
  '.md': 'text/markdown; charset=utf-8',
  '.svg': 'image/svg+xml'
};

function serveFile(res, filePath) {
  fs.readFile(filePath, (err, data) => {
    if (err) {
      res.writeHead(404, { 'Content-Type': 'text/plain; charset=utf-8', 'Cache-Control': 'no-store' });
      res.end('Not found');
      return;
    }
    const ext = path.extname(filePath).toLowerCase();
    res.writeHead(200, {
      'Content-Type': MIME[ext] || 'application/octet-stream',
      'Cache-Control': 'no-store'
    });
    res.end(data);
  });
}

const server = http.createServer((req, res) => {
  let url;
  try {
    url = new URL(req.url || '/', `http://127.0.0.1:${PORT}`);
  } catch {
    res.writeHead(400, { 'Content-Type': 'text/plain; charset=utf-8' });
    res.end('Bad request');
    return;
  }

  if (url.pathname === '/api/run') {
    const rawSeed = (url.searchParams.get('seed') || '2026001').trim();
    const seed = /^\d{1,19}$/.test(rawSeed) && rawSeed !== '0' ? rawSeed : '2026001';
    const rawMut = Number(url.searchParams.get('mutant') || 0);
    const mutant = String(Number.isFinite(rawMut) ? Math.max(0, Math.min(10, Math.trunc(rawMut))) : 0);
    const feral = url.searchParams.get('feral') === '1';
    const args = ['json', '--seed', seed, '--mutant', mutant];
    if (feral) args.push('--feral');

    execFile(BIN, args, { maxBuffer: 8 * 1024 * 1024, timeout: 15000 }, (err, stdout, stderr) => {
      if (err) {
        res.writeHead(500, { 'Content-Type': 'application/json; charset=utf-8', 'Cache-Control': 'no-store' });
        res.end(JSON.stringify({ error: err.message, stderr }));
        return;
      }
      res.writeHead(200, { 'Content-Type': 'application/json; charset=utf-8', 'Cache-Control': 'no-store' });
      res.end(stdout);
    });
    return;
  }

  if (url.pathname === '/api/lean') {
    serveFile(res, path.join(ROOT, 'results', 'lean4-omega-audit.txt'));
    return;
  }

  if (url.pathname === '/api/health') {
    res.writeHead(200, { 'Content-Type': 'application/json; charset=utf-8', 'Cache-Control': 'no-store' });
    res.end(JSON.stringify({ ok: true, version: '4.0', port: PORT, testbeds: 16, lean_theorems: 60, gates: 43, mutants: 10 }));
    return;
  }

  let decoded;
  try {
    decoded = decodeURIComponent(url.pathname);
  } catch {
    res.writeHead(400, { 'Content-Type': 'text/plain; charset=utf-8' });
    res.end('Bad path encoding');
    return;
  }
  const rel = decoded === '/' ? 'index.html' : decoded.replace(/^\/+/, '');
  const resolved = path.resolve(__dirname, rel);
  if (!resolved.startsWith(__dirname + path.sep) && resolved !== path.join(__dirname, 'index.html')) {
    res.writeHead(403, { 'Content-Type': 'text/plain; charset=utf-8' });
    res.end('Forbidden');
    return;
  }
  serveFile(res, resolved);
});

server.listen(PORT, '127.0.0.1', () => {
  console.log(`CELLULAR ENGINE OMEGA v4.0 (Deep Strata · Bio-Frostbite INet · Virtual-Cell AI · 16 Testbeds · 60 Lean 4 Proofs) running at http://127.0.0.1:${PORT}`);
});
