# Enlaceador - Qt Version

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
cd qt
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/msvc2022_64" -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
windeployqt --release main/Release/Enlaceador.exe
```

Output in `build/main/Release/` and `build/register/Release/`:
- `Enlaceador.exe` - Main GUI app
- `EnlaceadorRegister.exe` - Registry handler
- Qt DLLs (deployed by windeployqt)

## Installer

```bash
# Point Inno Setup to qt\Enlaceador-Qt.iss
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
| `Enlaceador-Qt.iss` | Inno Setup installer script |
