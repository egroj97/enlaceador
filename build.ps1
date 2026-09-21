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

function Find-QtTools {
    $qtBase = "C:\Qt"
    $found = @{
        QtRoot     = $null
        CMakeBin   = $null
        NinjaBin   = $null
        MinGwGcc   = $null
        MinGwBin   = $null
        QtBinDir   = $null
        WinDeployQt = $null
    }

    $found.QtRoot = $env:Qt6_DIR
    if (-not $found.QtRoot) { $found.QtRoot = $env:CMAKE_PREFIX_PATH }

    if (-not $found.QtRoot -and (Test-Path $qtBase)) {
        $versions = Get-ChildItem $qtBase -Directory -ErrorAction SilentlyContinue |
            Where-Object { $_.Name -match '^\d+\.\d+' } |
            Sort-Object { [version]($_.Name -replace '-.*','') } -Descending
        foreach ($ver in $versions) {
            $candidate = Join-Path $ver.FullName "mingw_64"
            if (Test-Path $candidate) {
                $found.QtRoot = $candidate
                break
            }
        }
    }

    if (-not $found.QtRoot -or !(Test-Path $found.QtRoot)) {
        return $null
    }

    $found.QtBinDir = Join-Path $found.QtRoot "bin"

    $toolsDir = Join-Path (Split-Path $found.QtRoot -Parent) "Tools"
    if (-not (Test-Path $toolsDir)) {
        $toolsDir = Join-Path (Split-Path (Split-Path $found.QtRoot -Parent) -Parent) "Tools"
    }

    if (Test-Path $toolsDir) {
        $toolDirs = Get-ChildItem $toolsDir -Directory -ErrorAction SilentlyContinue
        foreach ($td in $toolDirs) {
            if ($td.Name -match '^CMake' -and -not $found.CMakeBin) {
                $cmakeExe = Join-Path $td.FullName "bin\cmake.exe"
                if (Test-Path $cmakeExe) {
                    $found.CMakeBin = Join-Path $td.FullName "bin"
                }
            }
            elseif ($td.Name -match '^Ninja' -and -not $found.NinjaBin) {
                $ninjaExe = Join-Path $td.FullName "ninja.exe"
                if (Test-Path $ninjaExe) {
                    $found.NinjaBin = $td.FullName
                }
            }
            elseif ($td.Name -match '^mingw' -and -not $found.MinGwGcc) {
                $gccExe = Join-Path $td.FullName "bin\g++.exe"
                if (Test-Path $gccExe) {
                    $found.MinGwGcc = $gccExe
                    $found.MinGwBin = Join-Path $td.FullName "bin"
                }
            }
        }
    }

    if (Test-Path $found.QtBinDir) {
        $wdq = Join-Path $found.QtBinDir "windeployqt6.exe"
        if (!(Test-Path $wdq)) {
            $wdq = Join-Path $found.QtBinDir "windeployqt.exe"
        }
        if (Test-Path $wdq) {
            $found.WinDeployQt = $wdq
        }
    }

    return $found
}

function Build-Qt {
    Write-Host "`n  === Building Qt ===" -ForegroundColor Cyan

    $qt = Find-QtTools

    if (-not $qt) {
        Write-Host "  SKIP: Qt6 not found. Set Qt6_DIR or install Qt 6." -ForegroundColor Yellow
        $script:results += [PSCustomObject]@{ Version = "Qt"; Status = "SKIP"; Details = "Qt6 not found" }
        return
    }

    $pathPrepend = @()
    if ($qt.CMakeBin)   { $pathPrepend += $qt.CMakeBin }
    if ($qt.NinjaBin)   { $pathPrepend += $qt.NinjaBin }
    if ($qt.MinGwBin)   { $pathPrepend += $qt.MinGwBin }
    if ($qt.QtBinDir)   { $pathPrepend += $qt.QtBinDir }

    if ($pathPrepend.Count -gt 0) {
        $env:PATH = ($pathPrepend -join ";") + ";" + $env:PATH
    }

    if (!(Get-Command cmake -ErrorAction SilentlyContinue)) {
        Write-Host "  SKIP: cmake not found" -ForegroundColor Yellow
        $script:results += [PSCustomObject]@{ Version = "Qt"; Status = "SKIP"; Details = "cmake not found" }
        return
    }

    $qtRoot = $qt.QtRoot
    $mingwGcc = $qt.MinGwGcc

    Push-Location "$root"

    if (!(Test-Path "build")) {
        New-Item -ItemType Directory -Path "build" | Out-Null
    }

    Push-Location "build"

    $cmakeArgs = @("..", "-G", "Ninja", "-DCMAKE_PREFIX_PATH=$qtRoot", "-DCMAKE_BUILD_TYPE=Release", "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON")
    if ($mingwGcc) {
        $cmakeArgs += "-DCMAKE_CXX_COMPILER=$mingwGcc"
    }

    & cmake @cmakeArgs
    $configOk = $LASTEXITCODE -eq 0

    if ($configOk) {
        & cmake --build .
        $buildOk = $LASTEXITCODE -eq 0
    } else {
        $buildOk = $false
    }

    Pop-Location
    Pop-Location

    if ($configOk -and $buildOk) {
        $distDir = Join-Path $root "dist\installer"
        if (!(Test-Path $distDir)) {
            New-Item -ItemType Directory -Path $distDir -Force | Out-Null
        }
        
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
        $script:results += [PSCustomObject]@{ Version = "Qt"; Status = "OK"; Details = "$mainSize KB / $regSize KB" }
    } else {
        Write-Host "  FAIL: cmake build failed" -ForegroundColor Red
        $script:results += [PSCustomObject]@{ Version = "Qt"; Status = "FAIL"; Details = "cmake build failed" }
    }
}

function Build-Installer {
    Write-Host "`n  === Building Installer ===" -ForegroundColor Cyan

    $iscc = $null
    $candidates = @(
        "C:\Program Files (x86)\Inno Setup 6\ISCC.exe",
        "C:\Program Files\Inno Setup 6\ISCC.exe",
        "C:\Program Files (x86)\Inno Setup 5\ISCC.exe",
        "C:\Program Files\Inno Setup 5\ISCC.exe"
    )
    foreach ($c in $candidates) {
        if (Test-Path $c) { $iscc = $c; break }
    }

    if (-not $iscc) {
        $iscc = Get-Command iscc -ErrorAction SilentlyContinue | Select-Object -ExpandProperty Source
    }

    if (-not $iscc) {
        Write-Host "  SKIP: Inno Setup (ISCC.exe) not found. Install Inno Setup 6 to build the installer." -ForegroundColor Yellow
        $script:results += [PSCustomObject]@{ Version = "Installer"; Status = "SKIP"; Details = "ISCC.exe not found" }
        return
    }

    $issFile = Join-Path $root "enlaceador.iss"
    if (!(Test-Path $issFile)) {
        Write-Host "  SKIP: enlaceador.iss not found" -ForegroundColor Yellow
        $script:results += [PSCustomObject]@{ Version = "Installer"; Status = "SKIP"; Details = "enlaceador.iss not found" }
        return
    }

    Write-Host "  Using: $iscc" -ForegroundColor Gray
    & $iscc $issFile
    $isccOk = $LASTEXITCODE -eq 0

    if ($isccOk) {
        $setupExe = Join-Path $root "dist\installer\enlaceadorSetup.exe"
        if (Test-Path $setupExe) {
            $setupSize = [math]::Round((Get-Item $setupExe).Length / 1MB, 2)
            $md5 = (Get-FileHash -Algorithm MD5 $setupExe).Hash.ToLower()
            "$md5  enlaceadorSetup.exe" | Set-Content -NoNewline "$setupExe.md5"
            Write-Host "  OK: enlaceadorSetup.exe ($setupSize MB)" -ForegroundColor Green
            Write-Host "  MD5: $md5" -ForegroundColor Gray
            $script:results += [PSCustomObject]@{ Version = "Installer"; Status = "OK"; Details = "$setupSize MB" }
        } else {
            Write-Host "  OK: ISCC completed (output not found at expected path)" -ForegroundColor Yellow
            $script:results += [PSCustomObject]@{ Version = "Installer"; Status = "OK"; Details = "output not found" }
        }
    } else {
        Write-Host "  FAIL: ISCC compilation failed" -ForegroundColor Red
        $script:results += [PSCustomObject]@{ Version = "Installer"; Status = "FAIL"; Details = "ISCC compilation failed" }
    }
}

Show-Header
Build-Qt
if ($results | Where-Object { $_.Version -eq "Qt" -and $_.Status -eq "OK" }) {
    Build-Installer
}
