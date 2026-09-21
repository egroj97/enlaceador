[Setup]
AppName=enlaceador
AppVersion=1.0
DefaultDirName={autopf}\enlaceador
DefaultGroupName=enlaceador
OutputDir=dist\installer
OutputBaseFilename=enlaceadorSetup-Qt
Compression=lzma
SolidCompression=yes
PrivilegesRequired=lowest
UninstallDisplayIcon={app}\enlaceador.exe
InfoAfterText=This installer was created using Inno Setup (https://jrsoftware.org/isinfo.php)

[Files]
Source: "build\main\enlaceador.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "build\register\enlaceadorRegister.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "build\main\Qt6Core.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "build\main\Qt6Gui.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "build\main\Qt6Widgets.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "build\main\libgcc_s_seh-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "build\main\libstdc++-6.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "build\main\libwinpthread-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "build\main\platforms\qwindows.dll"; DestDir: "{app}\platforms"; Flags: ignoreversion
Source: "build\main\styles\qmodernwindowsstyle.dll"; DestDir: "{app}\styles"; Flags: ignoreversion

[Icons]
Name: "{group}\enlaceador"; Filename: "{app}\enlaceador.exe"
Name: "{autodesktop}\enlaceador"; Filename: "{app}\enlaceador.exe"

[Run]
Filename: "{app}\enlaceadorRegister.exe"; Description: "Register as default HTTP/HTTPS handler"; Flags: postinstall nowait skipifsilent runascurrentuser

[UninstallRun]
Filename: "{app}\enlaceadorRegister.exe"; Parameters: "--uninstall"; Flags: runascurrentuser

[UninstallDelete]
Type: filesandordirs; Name: "{app}"
