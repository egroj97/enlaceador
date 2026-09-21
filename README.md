# enlaceador - Qt Version

Cross-platform Qt implementation of enlaceador.

## Requirements

- Qt 6 (Widgets module)
- CMake 3.16+
- C++17 compiler (tested with GCC)

## Build

Using script (recommended):

```powershell
.\build.ps1
```

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

## Performance

| Metric | Value |
|--------|-------|
| Startup | ~100-150ms |
| Binary size | ~30-40MB (with Qt DLLs) |
| GUI | Qt Widgets |
| Runtime dep | Qt6Core.dll, Qt6Gui.dll, Qt6Widgets.dll, qwindows.dll |

## Files

| File | Purpose |
|------|---------|
| `main/main.cpp` | Entry point, URL parsing |
| `main/BrowserChooser.*` | Custom frameless widget |
| `main/BrowserDetector.*` | Browser detection |
| `register/main.cpp` | HTTP/HTTPS protocol registration |
| `enlaceador-Qt.iss` | Inno Setup installer script |
