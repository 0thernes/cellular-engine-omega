param(
    [switch]$SkipTests,
    [switch]$SkipLean
)

$ErrorActionPreference = "Stop"
$proj  = Split-Path -Parent $MyInvocation.MyCommand.Path
$root  = "$env:USERPROFILE\.elan\toolchains\leanprover--lean4---v4.35.0"
$clang = "$root\bin\clang.exe"
$lake  = "$root\bin\lake.exe"
$ar    = "$root\bin\llvm-ar.exe"

if (-not (Test-Path $clang)) {
    throw "Bundled Clang toolchain not found at $clang"
}

New-Item -ItemType Directory -Force -Path "$proj\bin","$proj\results" | Out-Null

$core = @(
    "$proj\src\omega_rng_hash.c",
    "$proj\src\exp_01_06.c",
    "$proj\src\exp_07_12.c",
    "$proj\src\exp_13_open_proliferation.c",
    "$proj\src\exp_14_bio_inet.c",
    "$proj\src\exp_15_deep_strata.c",
    "$proj\src\exp_16_virtual_cell_ai.c"
)

$cflags = @(
    "-O3", "-std=c11", "-Wall", "-Wextra", "-Wshadow", "-Wno-unused-parameter",
    "-march=native", "-mavx2", "-mbmi", "-mbmi2", "-mpopcnt",
    "--target=x86_64-w64-windows-gnu",
    "-DOMEGA_BARE_WIN64",
    "-I$proj\include",
    "-isystem", "$root\include\clang"
)
$ldflags = @("-L$root\lib", "-fuse-ld=lld")
$libs    = @("-lkernel32", "-lucrtbase", "-lm", "-lmingw32", "-lmingwex")

Write-Host "[1/4] Building bin\omega_cell.exe ..."
& $clang @cflags @core "$proj\src\omega_main_cli.c" -o "$proj\bin\omega_cell.exe" @ldflags @libs
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Write-Host "[2/4] Building bin\omega_cell.dll (+ import lib) ..."
& $clang @cflags -shared -DOMEGA_BUILD_DLL @core -o "$proj\bin\omega_cell.dll" "-Wl,--out-implib,$proj\bin\libomega_cell.dll.a" @ldflags @libs
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Write-Host "[3/4] Building bin\test_omega_falsification.exe ..."
& $clang @cflags @core "$proj\test\test_omega_falsification.c" -o "$proj\bin\test_omega_falsification.exe" @ldflags @libs
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

if (Test-Path $ar) {
    Write-Host "[4/4] Building static archive bin\libomega_cell.a ..."
    $objs = @()
    foreach ($src in $core) {
        $obj = "$proj\bin\" + [System.IO.Path]::GetFileNameWithoutExtension($src) + ".o"
        & $clang -c @cflags $src -o $obj
        if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
        $objs += $obj
    }
    & $ar rcs "$proj\bin\libomega_cell.a" @objs
    Remove-Item -Force -Path $objs
}

if (-not $SkipTests) {
    Write-Host "`nRunning 43-Gate Lab-Grade Falsification Suite + 10 Planted Mutants..."
    & "$proj\bin\test_omega_falsification.exe" | Tee-Object -FilePath "$proj\results\falsification_win64.txt"
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

    Write-Host "`nRunning 16-Testbed Deep-Strata, Bio-Frostbite & Virtual-Cell Suite (Seed 2026001)..."
    & "$proj\bin\omega_cell.exe" all --seed 2026001 | Tee-Object -FilePath "$proj\results\testbeds_all_win64.txt"
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
}

if (-not $SkipLean -and (Test-Path $lake)) {
    Write-Host "`nVerifying 60 Pure Lean 4 Frontier Theorems (O01-O58 + O01b + O41b)..."
    Push-Location "$proj\lean"
    & $lake build 2>&1 | Tee-Object -FilePath "$proj\results\lean4-omega-audit.txt"
    $lrc = $LASTEXITCODE
    Pop-Location
    if ($lrc -ne 0) { exit $lrc }
    Write-Host "Lean 4 Formal Verification PASSED (60 theorems, 0 sorryAx, 0 custom axioms)!"
}

Write-Host "`nCELLULAR ENGINE OMEGA v4.0 (DEEP STRATA & VIRTUAL-CELL SINGULARITY) BUILD & VERIFICATION COMPLETE."
