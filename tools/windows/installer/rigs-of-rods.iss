
#define BuildDir "C:\Git\rigs-of-rods\_build\bin\Release"

#define MyAppName "Rigs of Rods"
#define MyAppVersion "0.4.7.0"
#define MyAppVersionInt "0.4.7.0"
#define MyAppPublisher "AnotherFoxGuy"
#define MyAppURL "https://rigsofrods.org/"
#define MyAppExeName "RoRConfig.exe"

#include "thirdparty\it_download.iss"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{312BE7CD-13FB-4B37-8B25-C7011FC19C81}}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\Rigs of Rods {#MyAppVersion}
DefaultGroupName=Rigs of Rods {#MyAppVersion}
LicenseFile=COPYING.txt
OutputBaseFilename=Rigs-of-Rods-{#MyAppVersion}
;SetupIconFile=ror.ico
Compression=lzma2/ultra64
SolidCompression=yes
VersionInfoVersion={#MyAppVersionInt}
VersionInfoProductName=Rigs of Rods
VersionInfoProductVersion={#MyAppVersionInt}
VersionInfoProductTextVersion={#MyAppVersion}
VersionInfoCompany=Rigs of Rods community
VersionInfoDescription=Rigs of Rods soft-body physics simulator
VersionInfoTextVersion={#MyAppVersion}
VersionInfoCopyright=Copyright (C) 2005-2017 Rigs of Rods community
InternalCompressLevel=ultra64
LanguageDetectionMethod=locale

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "brazilianportuguese"; MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"
Name: "catalan"; MessagesFile: "compiler:Languages\Catalan.isl"
Name: "corsican"; MessagesFile: "compiler:Languages\Corsican.isl"
Name: "czech"; MessagesFile: "compiler:Languages\Czech.isl"
Name: "danish"; MessagesFile: "compiler:Languages\Danish.isl"
Name: "dutch"; MessagesFile: "compiler:Languages\Dutch.isl"
Name: "finnish"; MessagesFile: "compiler:Languages\Finnish.isl"
Name: "french"; MessagesFile: "compiler:Languages\French.isl"
Name: "german"; MessagesFile: "compiler:Languages\German.isl"
Name: "greek"; MessagesFile: "compiler:Languages\Greek.isl"
Name: "hebrew"; MessagesFile: "compiler:Languages\Hebrew.isl"
Name: "hungarian"; MessagesFile: "compiler:Languages\Hungarian.isl"
Name: "italian"; MessagesFile: "compiler:Languages\Italian.isl"
Name: "japanese"; MessagesFile: "compiler:Languages\Japanese.isl"
Name: "norwegian"; MessagesFile: "compiler:Languages\Norwegian.isl"
Name: "polish"; MessagesFile: "compiler:Languages\Polish.isl"
Name: "portuguese"; MessagesFile: "compiler:Languages\Portuguese.isl"
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"
Name: "scottishgaelic"; MessagesFile: "compiler:Languages\ScottishGaelic.isl"
Name: "serbiancyrillic"; MessagesFile: "compiler:Languages\SerbianCyrillic.isl"
Name: "serbianlatin"; MessagesFile: "compiler:Languages\SerbianLatin.isl"
Name: "slovenian"; MessagesFile: "compiler:Languages\Slovenian.isl"
Name: "spanish"; MessagesFile: "compiler:Languages\Spanish.isl"
Name: "turkish"; MessagesFile: "compiler:Languages\Turkish.isl"
Name: "ukrainian"; MessagesFile: "compiler:Languages\Ukrainian.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
;Source: "thirdparty\unzipper.dll"; Flags: dontcopy
Source: "{#BuildDir}\RoRConfig.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildDir}\Caelum.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildDir}\cg.dll"; DestDir: "{app}"; Flags: ignoreversion
;Source: "{#BuildDir}\input.map"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildDir}\libcurl.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildDir}\OgreMain.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildDir}\OgreMeshUpgrader.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildDir}\OgreOverlay.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildDir}\OgrePaging.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildDir}\OgreRTShaderSystem.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildDir}\OgreTerrain.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildDir}\OgreVolume.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildDir}\OgreXMLConverter.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildDir}\OpenAL32.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildDir}\openal-info.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildDir}\Plugin_BSPSceneManager.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildDir}\Plugin_CgProgramManager.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildDir}\Plugin_OctreeSceneManager.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildDir}\Plugin_OctreeZone.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildDir}\Plugin_ParticleFX.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildDir}\Plugin_PCZSceneManager.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildDir}\plugins.cfg"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildDir}\RenderSystem_Direct3D9.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildDir}\RenderSystem_GL.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildDir}\RoR.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildDir}\RoRConfig.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildDir}\CrashSender1403.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildDir}\CrashRpt1403.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildDir}\dbghelp.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildDir}\crashrpt_lang.ini"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildDir}\languages\*"; DestDir: "{app}\languages"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#BuildDir}\resources\*"; DestDir: "{app}\resources"; Flags: ignoreversion recursesubdirs createallsubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files
; config
Source: "Rigs of Rods 0.4\config\categories.cfg"; DestDir: "{userdocs}\Rigs of Rods 0.4\config"; Flags: onlyifdoesntexist uninsneveruninstall
Source: "Rigs of Rods 0.4\config\Controller__Xbox_360_Wireless_Receiver_for_Windows_.map"; DestDir: "{userdocs}\Rigs of Rods 0.4\config"; Flags: onlyifdoesntexist uninsneveruninstall
Source: "Rigs of Rods 0.4\config\editor.cfg"; DestDir: "{userdocs}\Rigs of Rods 0.4\config"; Flags: onlyifdoesntexist uninsneveruninstall
Source: "Rigs of Rods 0.4\config\ground_models.cfg"; DestDir: "{userdocs}\Rigs of Rods 0.4\config"; Flags: onlyifdoesntexist uninsneveruninstall
Source: "Rigs of Rods 0.4\config\inertia_models.cfg"; DestDir: "{userdocs}\Rigs of Rods 0.4\config"; Flags: onlyifdoesntexist uninsneveruninstall
Source: "Rigs of Rods 0.4\config\input.map"; DestDir: "{userdocs}\Rigs of Rods 0.4\config"; Flags: onlyifdoesntexist uninsneveruninstall
Source: "Rigs of Rods 0.4\config\Logitech_Formula_Force_EX_USB.map"; DestDir: "{userdocs}\Rigs of Rods 0.4\config"; Flags: onlyifdoesntexist uninsneveruninstall
Source: "Rigs of Rods 0.4\config\Logitech_Formula_Force_EX_USB.txt"; DestDir: "{userdocs}\Rigs of Rods 0.4\config"; Flags: onlyifdoesntexist uninsneveruninstall
Source: "Rigs of Rods 0.4\config\Logitech_G27.map"; DestDir: "{userdocs}\Rigs of Rods 0.4\config"; Flags: onlyifdoesntexist uninsneveruninstall
Source: "Rigs of Rods 0.4\config\Logitech_G27.txt"; DestDir: "{userdocs}\Rigs of Rods 0.4\config"; Flags: onlyifdoesntexist uninsneveruninstall
Source: "Rigs of Rods 0.4\config\ogre.cfg"; DestDir: "{userdocs}\Rigs of Rods 0.4\config"; Flags: onlyifdoesntexist uninsneveruninstall
Source: "Rigs of Rods 0.4\config\README.txt"; DestDir: "{userdocs}\Rigs of Rods 0.4\config"; Flags: onlyifdoesntexist uninsneveruninstall
Source: "Rigs of Rods 0.4\config\RGT_Force_Feedback_Pro.map"; DestDir: "{userdocs}\Rigs of Rods 0.4\config"; Flags: onlyifdoesntexist uninsneveruninstall
Source: "Rigs of Rods 0.4\config\RGT_Force_Feedback_Pro.txt"; DestDir: "{userdocs}\Rigs of Rods 0.4\config"; Flags: onlyifdoesntexist uninsneveruninstall
Source: "Rigs of Rods 0.4\config\rig_editor.cfg"; DestDir: "{userdocs}\Rigs of Rods 0.4\config"; Flags: onlyifdoesntexist uninsneveruninstall
Source: "Rigs of Rods 0.4\config\RoR.cfg"; DestDir: "{userdocs}\Rigs of Rods 0.4\config"; Flags: onlyifdoesntexist uninsneveruninstall
Source: "Rigs of Rods 0.4\config\Saitek_ST290_Pro.map"; DestDir: "{userdocs}\Rigs of Rods 0.4\config"; Flags: onlyifdoesntexist uninsneveruninstall
Source: "Rigs of Rods 0.4\config\Saitek_ST290_Pro.txt"; DestDir: "{userdocs}\Rigs of Rods 0.4\config"; Flags: onlyifdoesntexist uninsneveruninstall
Source: "Rigs of Rods 0.4\config\skidmarks.cfg"; DestDir: "{userdocs}\Rigs of Rods 0.4\config"; Flags: onlyifdoesntexist uninsneveruninstall
Source: "Rigs of Rods 0.4\config\torque_models.cfg"; DestDir: "{userdocs}\Rigs of Rods 0.4\config"; Flags: onlyifdoesntexist uninsneveruninstall
Source: "Rigs of Rods 0.4\config\wavefield.cfg"; DestDir: "{userdocs}\Rigs of Rods 0.4\config"; Flags: onlyifdoesntexist uninsneveruninstall
; packs
Source: "Rigs of Rods 0.4\vehicles\agora.zip"; DestDir: "{userdocs}\Rigs of Rods 0.4\vehicles"; Flags: onlyifdoesntexist uninsneveruninstall; Components: DefaultPack
Source: "Rigs of Rods 0.4\vehicles\dafsemi.zip"; DestDir: "{userdocs}\Rigs of Rods 0.4\vehicles"; Flags: onlyifdoesntexist uninsneveruninstall; Components: DefaultPack
Source: "Rigs of Rods 0.4\terrains\simple2-terrain.zip"; DestDir: "{userdocs}\Rigs of Rods 0.4\terrains"; Flags: onlyifdoesntexist uninsneveruninstall; Components: DefaultPack
Source: "ContentPack"; DestDir: "{userdocs}\Rigs of Rods 0.4\packs\"; Components: ContentPack; AfterInstall: ExtractZip('{tmp}\pack_contentpack04.zip', '{userdocs}\Rigs of Rods 0.4\packs\')
; Visual C++ Redistributable for Visual Studio 2015
Source: "E:\ror-thirdparty\vcredist_x86.exe"; DestDir: "{tmp}"
Source: "E:\ror-thirdparty\dxwebsetup.exe"; DestDir: "{tmp}"

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\RoR.exe"; Tasks: desktopicon
Name: "{commondesktop}\Rigs of Rods Configurator"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Flags: nowait postinstall skipifsilent; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"
Filename: "{tmp}\vcredist_x86.exe"; Parameters: "/Q"; StatusMsg: "Installing Visual Studio Redistributable"
Filename: "{tmp}\dxwebsetup.exe"; Parameters: "/Q"; StatusMsg: "Installing DirectX"

[Components]
Name: "DefaultPack"; Description: "Two vehicles & Two terrains"; Types: full compact
Name: "ContentPack"; Description: "ContentPack (Download)"; ExtraDiskSpaceRequired: 207618048; Types: full

[Dirs]
Name: "{userdocs}\Rigs of Rods 0.4"; Flags: uninsneveruninstall
Name: "{userdocs}\Rigs of Rods 0.4\cache"; Flags: uninsneveruninstall
Name: "{userdocs}\Rigs of Rods 0.4\config"; Flags: uninsneveruninstall
Name: "{userdocs}\Rigs of Rods 0.4\logs"; Flags: uninsneveruninstall
Name: "{userdocs}\Rigs of Rods 0.4\packs"; Flags: uninsneveruninstall
Name: "{userdocs}\Rigs of Rods 0.4\screenshots"; Flags: uninsneveruninstall
Name: "{userdocs}\Rigs of Rods 0.4\terrains"; Flags: uninsneveruninstall
Name: "{userdocs}\Rigs of Rods 0.4\vehicles"; Flags: uninsneveruninstall

[Registry]
Root: "HKCR"; Subkey: "rorserver"; ValueType: string; ValueData: "URL:Rigs of Rods Server"
Root: "HKCR"; Subkey: "rorserver"; ValueType: string; ValueName: "URL Protocol"
Root: "HKCR"; Subkey: "rorserver\shell\open\command"; ValueType: string; ValueData: """{app}\RoR.exe"" ""-joinserver=""%1"""""

[Code]
procedure unzip(src, target: AnsiString);
external 'unzip@files:unzipper.dll stdcall delayload';
  
procedure InitializeWizard();
begin
  ITD_Init();
  ITD_DownloadAfter(wpReady); 
end;
  
procedure ExtractZip(src, target : AnsiString);
begin
  unzip(ExpandConstant(src), ExpandConstant(target));
end;

procedure CurPageChanged(CurPageID: Integer);
begin
  if CurPageID = wpReady then
  begin   
    if IsComponentSelected('ContentPack') then 
    begin     
      ITD_AddFile('http://dl.rigsofrods.org/pack_contentpack04.zip', expandconstant('{tmp}\pack_contentpack04.zip'));
    end;
  end;
end;
