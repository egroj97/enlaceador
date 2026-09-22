[Setup]
AppName=enlaceador
AppVersion=1.0
DefaultDirName={autopf}\enlaceador
DefaultGroupName=enlaceador
OutputDir=dist\installer
OutputBaseFilename=enlaceadorSetup
Compression=lzma
SolidCompression=yes
PrivilegesRequired=lowest
UninstallDisplayIcon={app}\enlaceador.exe

[Files]
Source: "build\main\enlaceador.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "build\register\enlaceadorRegister.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "build\main\Qt6Core.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "build\main\Qt6Gui.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "build\main\Qt6Widgets.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "build\main\libgcc_s_seh-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "build\main\libstdc++-6.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "build\main\libwinpthread-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "build\main\generic\*.dll"; DestDir: "{app}\generic"; Flags: ignoreversion
Source: "build\main\iconengines\*.dll"; DestDir: "{app}\iconengines"; Flags: ignoreversion
Source: "build\main\platforms\*.dll"; DestDir: "{app}\platforms"; Flags: ignoreversion
Source: "build\main\styles\*.dll"; DestDir: "{app}\styles"; Flags: ignoreversion


[Icons]
Name: "{group}\enlaceador"; Filename: "{app}\enlaceador.exe"
Name: "{autodesktop}\enlaceador"; Filename: "{app}\enlaceador.exe"

[Run]
Filename: "{app}\enlaceadorRegister.exe"; Description: "Register as default HTTP/HTTPS handler"; Flags: postinstall nowait skipifsilent runascurrentuser

[UninstallRun]
Filename: "{app}\enlaceadorRegister.exe"; Parameters: "--uninstall"; Flags: runascurrentuser; RunOnceId: "UninstallRegister"

[UninstallDelete]
Type: filesandordirs; Name: "{app}"
