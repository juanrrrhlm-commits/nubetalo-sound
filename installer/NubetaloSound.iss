; Instalador de Nubetalo Sound. Compilar con tools\make_installer.ps1 (genera antes el zip de fuentes y las imágenes).
#define AppName "Nubetalo Sound"
#define AppVersion "1.0.0"
#define AppExe "NubetaloSound.exe"
#define Root ".."
#define Bin "..\app\bin\x64"
#define DriverSrc "..\app\Installer\Drivers\Version14\win10\x64"

[Setup]
AppId={{6B0E3C52-7A41-4D1E-9A57-2C8F51D3B0A9}
AppName={#AppName}
AppVersion={#AppVersion}
AppVerName={#AppName} {#AppVersion}
AppPublisher=Nubetalo
AppContact=juanrrrhlm@gmail.com
AppCopyright=Copyright (C) 2026 Nubetalo. Basado en FxSound (AGPL v3)
VersionInfoVersion={#AppVersion}
VersionInfoDescription={#AppName}
DefaultDirName={autopf}\Nubetalo Sound
DefaultGroupName=Nubetalo Sound
DisableProgramGroupPage=yes
UninstallDisplayIcon={app}\{#AppExe}
SetupIconFile=..\app\bin\x64\NubetaloSound.ico
WizardStyle=modern
WizardImageFile=assets\wizard_big.bmp
WizardSmallImageFile=assets\wizard_small.bmp
LicenseFile=..\LICENSE.txt
OutputDir=output
OutputBaseFilename=NubetaloSound-Setup-{#AppVersion}
Compression=lzma2/ultra
SolidCompression=yes
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
#ifdef TEST
PrivilegesRequired=lowest
#else
PrivilegesRequired=admin
#endif
CloseApplications=yes
RestartApplications=no
MinVersion=10.0

[Languages]
Name: "spanish"; MessagesFile: "compiler:Languages\Spanish.isl"
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"
Name: "driver"; Description: "Instalar el dispositivo de audio virtual (necesario si aún no lo tienes)"; GroupDescription: "Componentes:"; Check: not DriverInstalled

[Files]
Source: "{#Bin}\{#AppExe}"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#Bin}\Factsoft\*.fac"; DestDir: "{app}\Factsoft"; Flags: ignoreversion
Source: "{#Bin}\NubetaloSound.ico"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#Root}\LICENSE.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#Root}\NOTICE.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#Root}\PRIVACY.md"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#Root}\SECURITY.md"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#Root}\tools\IBMPlexSans-OFL.txt"; DestDir: "{app}\licenses"; Flags: ignoreversion
Source: "output\Nubetalo-Sound-{#AppVersion}-source.zip"; DestDir: "{app}\source"; Flags: ignoreversion
Source: "{#DriverSrc}\fxdevcon64.exe"; DestDir: "{app}\driver"; Flags: ignoreversion
Source: "{#DriverSrc}\fxvad.inf"; DestDir: "{app}\driver"; Flags: ignoreversion
Source: "{#DriverSrc}\fxvad.sys"; DestDir: "{app}\driver"; Flags: ignoreversion
Source: "{#DriverSrc}\fxvadntamd64.cat"; DestDir: "{app}\driver"; Flags: ignoreversion

[Icons]
Name: "{group}\Nubetalo Sound"; Filename: "{app}\{#AppExe}"; IconFilename: "{app}\NubetaloSound.ico"
Name: "{group}\Licencia y avisos"; Filename: "{app}\NOTICE.txt"
Name: "{autodesktop}\Nubetalo Sound"; Filename: "{app}\{#AppExe}"; IconFilename: "{app}\NubetaloSound.ico"; Tasks: desktopicon

[Run]
; Instala el controlador solo si no existe ya (por ejemplo, porque FxSound lo instaló antes).
Filename: "{app}\driver\fxdevcon64.exe"; Parameters: "install ""{app}\driver\fxvad.inf"""; WorkingDir: "{app}\driver"; \
  StatusMsg: "Instalando el dispositivo de audio virtual..."; Flags: runhidden waituntilterminated; Tasks: driver; Check: not DriverInstalled
Filename: "{app}\{#AppExe}"; Description: "Abrir Nubetalo Sound"; WorkingDir: "{app}"; Flags: nowait postinstall skipifsilent

[UninstallRun]
Filename: "{app}\driver\fxdevcon64.exe"; Parameters: "remove"; WorkingDir: "{app}\driver"; RunOnceId: "RemoveVirtualDevice"; \
  Flags: runhidden waituntilterminated skipifdoesntexist; Check: RemoveDriverOnUninstall

[UninstallDelete]
Type: filesandordirs; Name: "{app}\driver"

[Code]
var
  RemoveDriver: Boolean;

function DriverInstalled: Boolean;
begin
  Result := RegKeyExists(HKLM, 'SYSTEM\CurrentControlSet\Services\fxvad');
end;

function InitializeUninstall: Boolean;
begin
  Result := True;
  RemoveDriver := False;
  if DriverInstalled and not UninstallSilent then
    RemoveDriver := MsgBox('¿Quitar también el dispositivo de audio virtual?' + #13#10 + #13#10 +
      'Déjalo si usas otro programa que lo necesite (por ejemplo FxSound). Se puede volver a instalar después.',
      mbConfirmation, MB_YESNO or MB_DEFBUTTON2) = IDYES;
end;

function RemoveDriverOnUninstall: Boolean;
begin
  Result := RemoveDriver;
end;


