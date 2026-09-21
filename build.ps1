# enlaceador Build Script
# Build all versions of the browser chooser app
# Usage: .\build-all.ps1 [qt]

$ErrorActionPreference = "Continue"
$root = $PSScriptRoot
$results = @()

function Show-Header {
    Write-Host ""
    Write-Host "  enlaceador Build Script" -ForegroundColor Cyan
    Write-Host "  =======================" -ForegroundColor Cyan
    Write-Host ""
}

function Build-Qt {
    Write-Host "`n  === Building Qt ===" -ForegroundColor Cyan

    if (!(Get-Command cmake -ErrorAction SilentlyContinue)) {
        Write-Host "  SKIP: cmake not found" -ForegroundColor Yellow
        $script:results += [PSCustomObject]@{ Version = "Qt"; Status = "SKIP"; Details = "cmake not found" }
        return
    }

    $qtRoot = $env:Qt6_DIR
    if (-not $qtRoot) { $qtRoot = $env:CMAKE_PREFIX_PATH }

    if (-not $qtRoot) {
        $qtBase = "C:\Qt"
        if (Test-Path $qtBase) {
            $versions = Get-ChildItem $qtBase -Directory -ErrorAction SilentlyContinue |
                Where-Object { $_.Name -match '^\d+\.\d+' } |
                Sort-Object { [version]($_.Name -replace '-.*','') } -Descending
            foreach ($ver in $versions) {
                $candidate = Join-Path $ver.FullName "mingw_64"
                if (Test-Path $candidate) {
                    $qtRoot = $candidate
                    break
                }
            }
        }
    }

    if (-not $qtRoot -or !(Test-Path $qtRoot)) {
        Write-Host "  SKIP: Qt6 not found. Set Qt6_DIR or install Qt 6." -ForegroundColor Yellow
        $script:results += [PSCustomObject]@{ Version = "Qt"; Status = "SKIP"; Details = "Qt6 not found" }
        return
    }

    $mingwGcc = $null
    $mingwTools = "C:\Qt\Tools"
    if (Test-Path $mingwTools) {
        $mingwDir = Get-ChildItem $mingwTools -Directory -ErrorAction SilentlyContinue |
            Where-Object { $_.Name -match '^mingw' } |
            Sort-Object Name -Descending | Select-Object -First 1
        if ($mingwDir) {
            $gccPath = Join-Path $mingwDir.FullName "bin\g++.exe"
            if (Test-Path $gccPath) {
                $mingwGcc = $gccPath
                $mingwBin = Join-Path $mingwDir.FullName "bin"
                $env:PATH = "$mingwBin;" + $env:PATH
            }
        }
    }

    $ninjaPath = "C:\Qt\Tools\Ninja"
    if (Test-Path (Join-Path $ninjaPath "ninja.exe")) {
        $env:PATH = "$ninjaPath;" + $env:PATH
    }

    $qtBinDir = Join-Path $qtRoot "bin"
    if (Test-Path $qtBinDir) {
        $env:PATH = "$qtBinDir;" + $env:PATH
    }

    Push-Location "$root"

    if (!(Test-Path "build")) {
        New-Item -ItemType Directory -Path "build" | Out-Null
    }

    Push-Location "build"

    $cmakeArgs = @("..", "-G", "Ninja", "-DCMAKE_PREFIX_PATH=$qtRoot", "-DCMAKE_BUILD_TYPE=Release")
    if ($mingwGcc) {
        $cmakeArgs += "-DCMAKE_CXX_COMPILER=$mingwGcc"
    }

    $outConfig = & cmake @cmakeArgs 2>&1
    $configOk = $LASTEXITCODE -eq 0

    if ($configOk) {
        $outBuild = & cmake --build . 2>&1
        $buildOk = $LASTEXITCODE -eq 0
    } else {
        $buildOk = $false
    }

    $deployOk = $false
    if ($buildOk) {
        $winDeployQt = Join-Path $qtBinDir "windeployqt6.exe"
        if (!(Test-Path $winDeployQt)) {
            $winDeployQt = Join-Path $qtBinDir "windeployqt.exe"
        }
        $mainExePath = Join-Path (Get-Location) "main\enlaceador.exe"
        if ((Test-Path $winDeployQt) -and (Test-Path $mainExePath)) {
            $deployOut = & $winDeployQt --release $mainExePath 2>&1
            $deployOk = $LASTEXITCODE -eq 0
        }
    }

    Pop-Location
    Pop-Location

    if ($configOk -and $buildOk) {
        $mainExe = Join-Path "$root\build\main" "enlaceador.exe"
        $regExe  = Join-Path "$root\build\register" "enlaceadorRegister.exe"

        if (Test-Path $mainExe) {
            $mainSize = [math]::Round((Get-Item $mainExe).Length / 1KB, 1)
        } else {
            $mainSize = 0
        }

        if (Test-Path $regExe) {
            $regSize = [math]::Round((Get-Item $regExe).Length / 1KB, 1)
        } else {
            $regSize = 0
        }

        Write-Host "  OK: enlaceador.exe ($mainSize KB)" -ForegroundColor Green
        Write-Host "  OK: enlaceadorRegister.exe ($regSize KB)" -ForegroundColor Green
        if ($deployOk) {
            Write-Host "  OK: windeployqt completed" -ForegroundColor Green
        } else {
            Write-Host "  WARN: windeployqt failed or skipped" -ForegroundColor Yellow
        }
        $script:results += [PSCustomObject]@{ Version = "Qt"; Status = "OK"; Details = "$mainSize KB / $regSize KB" }
    } else {
        Write-Host "  FAIL: cmake build failed" -ForegroundColor Red
        if (!$configOk) { $outConfig | Write-Host }
        if (!$buildOk) { $outBuild | Write-Host }
        $script:results += [PSCustomObject]@{ Version = "Qt"; Status = "FAIL"; Details = "cmake build failed" }
    }
}

function Show-Summary {
    Write-Host "`n  =======================" -ForegroundColor Cyan
    Write-Host "  Build Summary" -ForegroundColor Cyan
    Write-Host "  =======================" -ForegroundColor Cyan
    Write-Host ""
    $results | Format-Table -AutoSize -Property Version, Status, Details
}

Show-Header
Build-Qt    
Show-Summary
