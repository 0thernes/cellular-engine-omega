import http from 'node:http';
import fs from 'node:fs';
import path from 'node:path';
import { execFile } from 'node:child_process';
import { fileURLToPath } from 'node:url';

const __dirname = path.dirname(fileURLToPath(import.meta.url));
const ROOT = path.resolve(__dirname, '..');
const BIN = path.join(ROOT, 'bin', process.platform === 'win32' ? 'omega_cell.exe' : 'omega_cell');
const PORT = Number(process.env.PORT || 8791);

const ALLOWED_STATIC = new Set(['index.html', 'app.js', 'styles.css']);
const MAX_INFLIGHT = 2;
const MAX_CACHE = 64;
let inflight = 0;
const resultCache = new Map();

const MIME = {
  '.html': 'text/html; charset=utf-8',
  '.css': 'text/css; charset=utf-8',
  '.js': 'application/javascript; charset=utf-8',
  '.json': 'application/json; charset=utf-8',
  '.txt': 'text/plain; charset=utf-8'
};

function secHeaders(extra = {}) {
  return {
    'Cache-Control': 'no-store',
    'X-Content-Type-Options': 'nosniff',
    'X-Frame-Options': 'DENY',
    'Referrer-Policy': 'no-referrer',
    'Content-Security-Policy':
      "default-src 'self'; script-src 'self'; style-src 'self' 'unsafe-inline'; img-src 'self' data:; connect-src 'self'; object-src 'none'; frame-ancestors 'none'; base-uri 'self'",
    ...extra
  };
}

function isLoopbackHost(hostHeader) {
  if (!hostHeader || typeof hostHeader !== 'string') return false;
  const h = hostHeader.trim().toLowerCase();
  const validHosts = new Set([
    `127.0.0.1:${PORT}`,
    `localhost:${PORT}`,
    `[::1]:${PORT}`,
    '127.0.0.1',
    'localhost',
    '[::1]'
  ]);
  return validHosts.has(h);
}

function isAllowedOrigin(originHeader) {
  if (!originHeader) return true;
  const o = String(originHeader).trim().toLowerCase();
  return o === `http://127.0.0.1:${PORT}` ||
         o === `http://localhost:${PORT}` ||
         o === `http://[::1]:${PORT}`;
}

function isAllowedFetchSite(req, pathname) {
  const sfs = req.headers['sec-fetch-site'];
  if (!sfs) return true;
  const v = String(sfs).trim().toLowerCase();
  if (v === 'same-origin' || v === 'none') return true;
  /* Allow local loopback Hub (:8790) top-level navigation to index.html, never to /api/* */
  if (v === 'same-site' &&
      String(req.headers['sec-fetch-mode'] || '').toLowerCase() === 'navigate' &&
      (pathname === '/' || pathname === '/index.html')) {
    return true;
  }
  return false;
}

function serveFile(res, filePath, headOnly = false) {
  fs.readFile(filePath, (err, data) => {
    if (err) {
      res.writeHead(404, secHeaders({ 'Content-Type': 'text/plain; charset=utf-8' }));
      res.end(headOnly ? undefined : 'Not found');
      return;
    }
    const ext = path.extname(filePath).toLowerCase();
    res.writeHead(200, secHeaders({
      'Content-Type': MIME[ext] || 'application/octet-stream'
    }));
    res.end(headOnly ? undefined : data);
  });
}

function cacheSet(key, value) {
  if (resultCache.has(key)) resultCache.delete(key);
  resultCache.set(key, value);
  if (resultCache.size > MAX_CACHE) {
    const oldest = resultCache.keys().next().value;
    resultCache.delete(oldest);
  }
}

const server = http.createServer((req, res) => {
  const method = (req.method || 'GET').toUpperCase();
  if (method !== 'GET' && method !== 'HEAD') {
    res.writeHead(405, secHeaders({ 'Content-Type': 'text/plain; charset=utf-8', 'Allow': 'GET, HEAD' }));
    res.end('Method Not Allowed');
    return;
  }
  const headOnly = method === 'HEAD';

  let url;
  try {
    url = new URL(req.url || '/', `http://127.0.0.1:${PORT}`);
  } catch {
    res.writeHead(400, secHeaders({ 'Content-Type': 'text/plain; charset=utf-8' }));
    res.end('Bad request');
    return;
  }

  if (!isLoopbackHost(req.headers.host) ||
      !isAllowedOrigin(req.headers.origin) ||
      !isAllowedFetchSite(req, url.pathname)) {
    res.writeHead(403, secHeaders({ 'Content-Type': 'text/plain; charset=utf-8' }));
    res.end('Forbidden');
    return;
  }

  if (url.pathname === '/api/run') {
    const rawSeed = (url.searchParams.get('seed') || '2026001').trim();
    const seed = /^\d{1,19}$/.test(rawSeed) && rawSeed !== '0' ? rawSeed : '2026001';
    const rawMut = Number(url.searchParams.get('mutant') || 0);
    const mutant = String(Number.isFinite(rawMut) ? Math.max(0, Math.min(16, Math.trunc(rawMut))) : 0);
    const feral = url.searchParams.get('feral') === '1';
    const cacheKey = `${seed}:${mutant}`;

    if (!feral && resultCache.has(cacheKey)) {
      const cached = resultCache.get(cacheKey);
      res.writeHead(200, secHeaders({
        'Content-Type': 'application/json; charset=utf-8',
        'X-Omega-Cache': 'HIT'
      }));
      res.end(headOnly ? undefined : cached);
      return;
    }

    if (inflight >= MAX_INFLIGHT) {
      res.writeHead(429, secHeaders({
        'Content-Type': 'application/json; charset=utf-8',
        'Retry-After': '1'
      }));
      res.end(headOnly ? undefined : JSON.stringify({ error: 'Too many concurrent engine runs (max 2 in-flight)' }));
      return;
    }

    const args = ['json', '--seed', seed, '--mutant', mutant];
    if (feral) args.push('--feral');

    inflight++;
    execFile(BIN, args, { maxBuffer: 8 * 1024 * 1024, timeout: 15000 }, (err, stdout, stderr) => {
      inflight = Math.max(0, inflight - 1);
      if (err) {
        res.writeHead(500, secHeaders({ 'Content-Type': 'application/json; charset=utf-8' }));
        res.end(headOnly ? undefined : JSON.stringify({ error: err.message, stderr }));
        return;
      }
      if (!feral) cacheSet(cacheKey, stdout);
      res.writeHead(200, secHeaders({
        'Content-Type': 'application/json; charset=utf-8',
        'X-Omega-Cache': feral ? 'BYPASS' : 'MISS'
      }));
      res.end(headOnly ? undefined : stdout);
    });
    return;
  }

  if (url.pathname === '/api/lean') {
    serveFile(res, path.join(ROOT, 'results', 'lean4-omega-audit.txt'), headOnly);
    return;
  }

  if (url.pathname === '/api/cube') {
    serveFile(res, path.join(ROOT, 'results', 'omega_75_substrates_1000_cube.json'), headOnly);
    return;
  }

  if (url.pathname === '/api/health') {
    res.writeHead(200, secHeaders({ 'Content-Type': 'application/json; charset=utf-8' }));
    res.end(headOnly ? undefined : JSON.stringify({
      ok: true,
      version: '5.1.0',
      port: PORT,
      testbeds: 16,
      substrates_75: 75,
      hybrid_cube_1000: 1000,
      lean_theorems: 72,
      gates: 54,
      mutants: 16
    }));
    return;
  }

  let decoded;
  try {
    decoded = decodeURIComponent(url.pathname);
  } catch {
    res.writeHead(400, secHeaders({ 'Content-Type': 'text/plain; charset=utf-8' }));
    res.end('Bad path encoding');
    return;
  }

  const rel = decoded === '/' ? 'index.html' : decoded.replace(/^\/+/, '');
  if (!ALLOWED_STATIC.has(rel)) {
    res.writeHead(404, secHeaders({ 'Content-Type': 'text/plain; charset=utf-8' }));
    res.end(headOnly ? undefined : 'Not found');
    return;
  }
  serveFile(res, path.join(__dirname, rel), headOnly);
});

server.listen(PORT, '127.0.0.1', () => {
  console.log(`CELLULAR ENGINE OMEGA v5.1.0 (75 Substrates · 1,000 Cube Cells · Live C11->Lean O63/O72 · 16 Testbeds · 72 Lean 4 Proofs · 54 Gates / 16 Mutants) running at http://127.0.0.1:${PORT}`);
});
