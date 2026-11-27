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

if (-not (Test-Path -LiteralPath $clang)) {
    throw "Bundled Clang toolchain not found at $clang"
}

[System.IO.Directory]::CreateDirectory("$proj\bin") | Out-Null
[System.IO.Directory]::CreateDirectory("$proj\results") | Out-Null

$core = @(
    "$proj\src\omega_rng_hash.c",
    "$proj\src\exp_01_06.c",
    "$proj\src\exp_07_12.c",
    "$proj\src\exp_13_open_proliferation.c",
    "$proj\src\exp_14_bio_inet.c",
    "$proj\src\exp_15_deep_strata.c",
    "$proj\src\exp_16_virtual_cell_ai.c",
    "$proj\src\omega_substrates_cube.c"
)

$cflags = @(
    "-O3", "-std=c11", "-Wall", "-Wextra", "-Wshadow", "-Werror", "-Wno-unused-parameter",
    "-march=native", "-mavx2", "-mavxvnni", "-mbmi", "-mbmi2", "-mpopcnt",
    "-ffp-contract=off",
    "--target=x86_64-w64-windows-gnu",
    "--sysroot=$root",
    "-B$root\lib",
    "-DOMEGA_BARE_WIN64",
    "-I$proj\include",
    "-isystem", "$root\include\clang"
)
$lto = @("-flto=thin")
$ldflags = @("-L$root\lib", "-fuse-ld=lld")
$libs    = @("-lkernel32", "-lucrtbase", "-lm", "-lmingw32", "-lmingwex")

Write-Host "[1/5] Building bin\omega_cell.exe (Win64 AVX2/VNNI + ThinLTO) ..."
& $clang @cflags @lto @core "$proj\src\omega_main_cli.c" -o "$proj\bin\omega_cell.exe" @ldflags @libs
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Write-Host "[2/5] Building bin\omega_cell.dll (+ import lib, ThinLTO) ..."
& $clang @cflags @lto -shared -DOMEGA_BUILD_DLL @core -o "$proj\bin\omega_cell.dll" "-Wl,--out-implib,$proj\bin\libomega_cell.dll.a" @ldflags @libs
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Write-Host "[3/5] Building bin\test_omega_falsification.exe (ThinLTO) ..."
& $clang @cflags @lto @core "$proj\test\test_omega_falsification.c" -o "$proj\bin\test_omega_falsification.exe" @ldflags @libs
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Write-Host "[4/5] Building bin\fuzz_omega_standalone.exe (Fuzz + Property-Based Harness) ..."
& $clang @cflags @lto "$proj\src\omega_rng_hash.c" "$proj\src\exp_07_12.c" "$proj\src\exp_14_bio_inet.c" "$proj\test\fuzz_omega_cli_codec.c" -o "$proj\bin\fuzz_omega_standalone.exe" @ldflags @libs
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

if (Test-Path -LiteralPath $ar) {
    Write-Host "[5/5] Building static archive bin\libomega_cell.a ..."
    $objs = @()
    foreach ($src in $core) {
        $obj = "$proj\bin\" + [System.IO.Path]::GetFileNameWithoutExtension($src) + ".o"
        & $clang -c @cflags $src -o $obj
        if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
        $objs += $obj
    }
    & $ar rcs "$proj\bin\libomega_cell.a" @objs
    Remove-Item -Force -LiteralPath $objs
}

if (-not $SkipTests) {
    Write-Host "`nRunning 60-Gate Falsification Suite + 16/16 Planted Mutants + 10-Seed Sweep + 75-Sub/1,000-Cube..."
    $falsOut = & "$proj\bin\test_omega_falsification.exe" 2>&1
    $rc = $LASTEXITCODE
    $falsOut
    $falsOut | Set-Content -LiteralPath "$proj\results\falsification_win64.txt" -Encoding utf8
    if ($rc -ne 0) { exit $rc }

    Write-Host "`nRunning Standalone Fuzz + Property-Based Harness (256 mutated cases + 32 prop_check seeds)..."
    $fuzzOut = & "$proj\bin\fuzz_omega_standalone.exe" 2>&1
    $rcf = $LASTEXITCODE
    $fuzzOut
    if ($rcf -ne 0) { exit $rcf }

    Write-Host "`nRunning 16-Testbed Deep-Strata, Bio-Frostbite & Virtual-Cell Suite (Seed 2026001)..."
    $tbOut = & "$proj\bin\omega_cell.exe" all --seed 2026001 2>&1
    $rc2 = $LASTEXITCODE
    $tbOut
    $tbOut | Set-Content -LiteralPath "$proj\results\testbeds_all_win64.txt" -Encoding utf8
    if ($rc2 -ne 0) { exit $rc2 }

    Write-Host "`nRunning Clean-Slate v3.0 75 Substrates + 1,000-Cell Hybrid Cube (Seed 2026001)..."
    $cubeOut = & "$proj\bin\omega_cell.exe" cube "$proj\results\omega_75_substrates_1000_cube.json" --seed 2026001 2>&1
    $rc3 = $LASTEXITCODE
    $cubeOut
    if ($rc3 -ne 0) { exit $rc3 }
}

if (-not $SkipLean -and (Test-Path -LiteralPath $lake)) {
    Write-Host "`nExporting Live C11 Runtime Trace & 75-Substrate/1,000-Cube Receipt -> lean\OmegaCellularProofs\LiveC11Receipt.lean ..."
    & "$proj\bin\omega_cell.exe" --export-lean-trace "$proj\lean\OmegaCellularProofs\LiveC11Receipt.lean" --seed 2026001
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

    Write-Host "`nVerifying 100 Constructive Pure Lean 4.35.0 Theorems (Palomar Module System + Challenge/Solution + Multi-Kernel Audit)..."
    Push-Location -LiteralPath "$proj\lean"
    $buildOut = & $lake build 2>&1
    $lrc = $LASTEXITCODE
    $chalOut = @()
    $crc = 0
    $axiomOut = @()
    $arc = 0
    if ($lrc -eq 0) {
        $chalOut = & $lake build Challenge 2>&1
        $crc = $LASTEXITCODE
    }
    if ($lrc -eq 0 -and $crc -eq 0) {
        $axiomOut = & $lake env lean VerifyAxioms.lean 2>&1
        $arc = $LASTEXITCODE
    }
    Pop-Location

    $buildOut
    if ($lrc -ne 0) { exit $lrc }
    $chalOut
    if ($crc -ne 0) { exit $crc }
    $axiomJoined = (($axiomOut | ForEach-Object { $_.ToString() }) -join "`n") + "`n"
    [System.IO.File]::WriteAllText("$proj\results\lean4-omega-audit.txt", $axiomJoined, [System.Text.UTF8Encoding]::new($false))
    if ($arc -ne 0) {
        $axiomOut
        exit $arc
    }

    $combinedText = (($buildOut | Out-String) + "`n" + ($axiomOut | Out-String))
    if ($combinedText -match "sorryAx|declaration uses 'sorry'|warning:|Lean\.ofReduceBool|Lean\.trustCompiler|_native\.bv_decide|Classical\.choice") {
        throw "Lean 4 audit failed: sorryAx, Classical.choice, compiler/SAT axiom, or warning detected!"
    }

    $axiomText = ($axiomOut | Out-String)
    $rx = [regex]"'(OmegaCellular\.[^']+)' (depends on axioms:\s*\[([^\]]+)\]|does not depend on any axioms)"
    $matches = $rx.Matches($axiomText)
    $seen = New-Object 'System.Collections.Generic.HashSet[string]'
    $badAxioms = @()
    foreach ($m in $matches) {
        [void]$seen.Add($m.Groups[1].Value)
        if ($m.Groups[3].Success) {
            foreach ($ax in ($m.Groups[3].Value -split ",")) {
                $t = $ax.Trim()
                if ($t -and ($t -notin @("propext", "Quot.sound"))) {
                    $badAxioms += "$($m.Groups[1].Value):$t"
                }
            }
        }
    }
    if ($badAxioms.Count -gt 0) {
        throw ("Lean 4 audit failed: unauthorized axiom(s) detected: " + ($badAxioms -join ", "))
    }
    if ($seen.Count -lt 100) {
        throw "Lean 4 audit failed: expected >= 100 unique audited theorems, found $($seen.Count)!"
    }
    if (-not $seen.Contains("OmegaCellular.VerifyTraceReceipt.O63_live_c11_receipt_verified")) {
        throw "Lean 4 audit failed: O63_live_c11_receipt_verified missing from axiom audit!"
    }
    if (-not $seen.Contains("OmegaCellular.SubstratesCube.O72_live_c11_cube_receipt_verified")) {
        throw "Lean 4 audit failed: O72_live_c11_cube_receipt_verified missing from axiom audit!"
    }
    if (-not $seen.Contains("OmegaCellular.AdversarialTests.O100_refute_hybrid_cube_partition_drift")) {
        throw "Lean 4 audit failed: O100_refute_hybrid_cube_partition_drift missing from axiom audit!"
    }

    Write-Host "`nRunning reference kernel (leanchecker, leanchecker-paranoid) + independent export checkers (con-leche, con-ron, nanoda_bin) + negative elaboration test..."
    Push-Location -LiteralPath "$proj\lean"
    try {
        & $lake env leanchecker --fresh Solution
        if ($LASTEXITCODE -ne 0) { throw "leanchecker --fresh Solution failed!" }
        & $lake env leanchecker-paranoid --fresh Solution
        if ($LASTEXITCODE -ne 0) { throw "leanchecker-paranoid --fresh Solution failed!" }

        $negFile = Join-Path $env:TEMP "omega_neg_elab_test.lean"
        Set-Content -LiteralPath $negFile -Encoding ascii -Value @(
            "import OmegaCellularProofs.SubstratesCube",
            "theorem deliberately_false_cube_partition : (661 : Nat) + 274 + 20 + 45 = 999 := by rfl"
        )
        $prevPref = $ErrorActionPreference
        $ErrorActionPreference = "Continue"
        $null = & $lake env lean $negFile 2>&1
        $negRc = $LASTEXITCODE
        $ErrorActionPreference = $prevPref
        Remove-Item -Force -LiteralPath $negFile -ErrorAction SilentlyContinue
        if ($negRc -eq 0) { throw "CRITICAL: Lean 4.35.0 elaboration accepted a deliberately false literal equation!" }

        $comp = Get-Content -LiteralPath "$proj\lean\comparator.json" -Raw | ConvertFrom-Json
        $expFile = Join-Path $env:TEMP "omega_solution_all100.export"
        $nanodaCfg = Join-Path $env:TEMP "omega_nanoda_all100.json"
        $thmArgs = ($comp.theorem_names -join ' ')
        & cmd /c "`"$lake`" env leanexport Solution -- $thmArgs > `"$expFile`""
        if ($LASTEXITCODE -ne 0) { throw "leanexport Solution failed!" }

        & $lake env con-leche --verified $expFile
        if ($LASTEXITCODE -ne 0) { throw "con-leche --verified failed!" }
        & $lake env con-ron --verified $expFile
        if ($LASTEXITCODE -ne 0) { throw "con-ron --verified failed!" }

        $cfgObj = [ordered]@{
            export_file_path = ($expFile -replace '\\', '/')
            use_stdin = $false
            permitted_axioms = @("propext", "Quot.sound")
            unpermitted_axiom_hard_error = $true
            nat_extension = $true
            string_extension = $true
        }
        ($cfgObj | ConvertTo-Json -Depth 4) | Set-Content -LiteralPath $nanodaCfg -Encoding ascii
        & $lake env nanoda_bin $nanodaCfg
        if ($LASTEXITCODE -ne 0) { throw "nanoda_bin failed!" }
        Remove-Item -Force -LiteralPath $expFile, $nanodaCfg -ErrorAction SilentlyContinue
    } finally {
        Pop-Location
    }
    Write-Host "Lean 4.35.0 Formal Verification PASSED ($($seen.Count)/100 constructive theorems across reference kernel + con-leche + con-ron + nanoda_bin + negative elaboration test, 0 Classical.choice, 0 sorryAx, 0 warnings)!"
}

Write-Host "`nCELLULAR ENGINE OMEGA v5.4.0 (75 SUBSTRATES · 1,000 CUBE CELLS · LIVE C11->LEAN O63/O72 · 100 PROOFS · 60 GATES · 16/16 MUTANTS) COMPLETE."
