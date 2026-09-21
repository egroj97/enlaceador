# enlaceador

> **Disclaimer:** This program's development was AI-assisted (OpenCode Go, Qwen 2.5/3.7 Max).

A locally installed browser muxer for those who use different browsers for different facets of their lives.

## Download

You can find the installer alongside the MD5 checksum on the [GitHub Releases page](https://github.com/egroj97/enlaceador/releases).

## Build

### Requirements

- **Qt 6** (Widgets module)
- **CMake** 3.16+
- **C++17 compiler** (tested with GCC)

#### Using Script (Recommended)

```powershell
.\build.ps1

Manual:

```bash
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_PREFIX_PATH=<PATH_TO_QT_INSTALL> -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=<PATH_TO_CPP_COMPILTER>
cmake --build . --config Release
```

```powershell
# windows only
windeployqt --release main/Release/enlaceador.exe
```

Output in `build/main/Release/` and `build/register/Release/`:
- `enlaceador.exe` - Main GUI app
- `enlaceadorRegister.exe` - Registry handler
- Qt DLLs (deployed by windeployqt)

## Installer

```bash
# Point Inno Setup to qt\enlaceador-Qt.iss
# Ensure windeployqt has been run first
```