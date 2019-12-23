[Languages]
Name: "de"; MessagesFile: "German.isl"

[Setup]
AppName=Quero Toolbar
AppVerName=Quero Toolbar 8 Build [x64] Windows 8, Windows 7, Windows Vista
VersionInfoVersion=8.0.0.0
AppPublisher=Copyright 2013 Viktor Krammer
AppPublisherURL=http://www.quero.at/
AppSupportURL=http://www.quero.at/
AppUpdatesURL=http://www.quero.at/
CreateAppDir=yes
DefaultDirName={pf}\Quero Toolbar
UsePreviousAppDir=yes
LicenseFile=license.txt
; InfoBeforeFile=before.txt
; InfoAfterFile=success.txt
Compression=lzma
SolidCompression=yes
MinVersion=0,6
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
PrivilegesRequired=admin
OutputBaseFilename=QueroToolbarInstaller_DE_x64
CloseApplications=yes
RestartApplications=no
WizardImageFile=E:\Data\Web\quero\intern\setup_big.bmp
WizardSmallImageFile=E:\Data\Web\quero\intern\setup_small.bmp

[Files]
Source:"..\Build\output\QueroBroker\x86\QueroBroker.dll"; DestDir:"{app}"; Flags:ignoreversion;
Source:"..\Build\output\QueroBroker\x86\QueroBroker.exe"; DestDir:"{app}"; AfterInstall:RegisterQueroBroker; Flags:ignoreversion;
Source:"..\Build\output\QueroBroker\x64\QueroBroker.dll"; DestDir:"{app}\x64"; Flags:ignoreversion;
Source:"..\Build\output\QueroBroker\x64\QueroBroker.exe"; DestDir:"{app}\x64"; AfterInstall:RegisterQueroBroker; Flags:ignoreversion;
Source:"..\Profiles\8000\QueroProfiles_DE.reg"; DestDir:"{app}"; AfterInstall:InstallSearchProfiles; Flags:deleteafterinstall ignoreversion;
Source:"..\Build\output\de_x86\Quero.dll"; DestDir:"{app}"; Flags:ignoreversion regserver 32bit;
Source:"..\Build\output\de_x64\Quero.dll"; DestDir:"{app}\x64"; Flags:ignoreversion regserver 64bit;

[UninstallRun]
Filename:"{app}\QueroBroker.exe"; Parameters:"/UnregServer";
Filename:"{app}\x64\QueroBroker.exe"; Parameters:"/UnregServer";

[Run]
Filename:"{pf}\Internet Explorer\iexplore.exe"; Parameters:"res://{app}\x64\Quero.dll/installed.html"; Description: "Internet Explorer öffnen und Quero-Tipps anzeigen"; Flags:postinstall skipifsilent nowait;

[Code]
const
  VISTA_BUILD = true;
  QUERO_HKCU_KEY_SOFTWARE = 'Software\Quero Toolbar';
  QUERO_HKCU_KEY_APPDATALOW = 'Software\AppDataLow\Software\Quero Toolbar';
  QUERO_HKCU_KEY_INTERNETREGISTRY = 'Software\Microsoft\Internet Explorer\InternetRegistry\Software\Quero Toolbar';
const
(* English
  TXT_SEARCH_DEFAULTS = 'Search Defaults';
  TXT_CHOOSE_SEARCH_DEFAULTS = 'Please choose your default search profile and search engine.';
  TXT_DEFAULT_SEARCH_PROFILE = 'Default search profile:';
  TXT_PROFILE_0 = 'Standard - U.S.';
  TXT_PROFILE_1 = 'Austria';
  TXT_PROFILE_2 = 'Bulgaria';
  TXT_PROFILE_3 = 'China';
  TXT_PROFILE_4 = 'Finland';
  TXT_PROFILE_5 = 'France';
  TXT_PROFILE_6 = 'Germany';
  TXT_PROFILE_7 = 'Russia';
  TXT_PROFILE_8 = 'Switzerland';
  TXT_PROFILE_9 = 'United Kingdom';
  TXT_PROFILE_10 = 'Science';
  TXT_DEFAULT_SEARCH_ENGINE = 'Default search engine:';
  TXT_GOOGLE = 'Google';
  TXT_BING = 'Bing';
  TXT_YAHOO = 'Yahoo!';
  TXT_SEARCH_DEFAULTS_NOTE = 'Note: You can always change your search defaults in the Quero Toolbar menu.';
  TXT_UPDATE_SEARCH_PROFILE = 'Update search profiles:';
  TXT_YES = 'Yes';
  TXT_NO = 'No';
  TXT_REMOVE_INSTALL = 'Remove old installation:';
  TXT_TIDY_UP = 'Tidy up system directory';
  TXT_X86_INSTALL = 'x86 installation';
  TXT_CLOSE_IE = 'Close Internet Explorer';
(* German *)
  TXT_SEARCH_DEFAULTS = 'Suchstandard';
  TXT_CHOOSE_SEARCH_DEFAULTS = 'Bitte wählen Sie Ihr Suchprofil und Ihre Standard-Suchmaschine aus.';
  TXT_DEFAULT_SEARCH_PROFILE = 'Standard-Suchprofil:';
  TXT_PROFILE_0 = 'Standard - USA';
  TXT_PROFILE_1 = 'Österreich';
  TXT_PROFILE_2 = 'Bulgarien';
  TXT_PROFILE_3 = 'China';
  TXT_PROFILE_4 = 'Finnland';
  TXT_PROFILE_5 = 'Frankreich';
  TXT_PROFILE_6 = 'Deutschland';
  TXT_PROFILE_7 = 'Russland';
  TXT_PROFILE_8 = 'Schweiz';
  TXT_PROFILE_9 = 'U.K.';
  TXT_PROFILE_10 = 'Informatik';
  TXT_DEFAULT_SEARCH_ENGINE = 'Standard-Suchmaschine:';
  TXT_GOOGLE = 'Google';
  TXT_BING = 'Bing';
  TXT_YAHOO = 'Yahoo!';
  TXT_SEARCH_DEFAULTS_NOTE = 'Hinweis: Sie können Ihre Such-Einstellungen jederzeit im Quero Toolbar-Menü ändern.';
  TXT_UPDATE_SEARCH_PROFILE = 'Suchprofile aktualisieren:';
  TXT_YES = 'Ja';
  TXT_NO = 'Nein';
  TXT_REMOVE_INSTALL = 'Alte Installation entfernen:';
  TXT_TIDY_UP = 'System-Verzeichnis aufräumen';
  TXT_X86_INSTALL = 'x86-Installation';
  TXT_CLOSE_IE = 'Internet Explorer schließen';
const
  DEFAULT_ENGINES_COUNT = 3;
  PROFILE_COUNT = 11;
  PROFILE_ID_SCIENCE = 5;
  PROFILE_DEFAULTENGINE_MODIFIED = $8;
  PROFILES_CUSTOM = $10000;
// Global variables
var
  PageID_SearchProfile: Integer;
  ComboBox_DefaultProfile: TComboBox;
  RadioButtons_DefaultEngine: array [0..DEFAULT_ENGINES_COUNT-1] of TRadioButton;
  QueroVersion: Integer;
  bUpdateProfiles: Boolean;
  bSetSearchDefaults: Boolean;
  bTidyUp: Boolean;
  bRemove_x86: Boolean;
  Quero_HKCU_Key: String;
  WindowsVersion: Cardinal;

procedure TidyUpSysDir();
var
  UninstallString,FileName: String;
begin
  // Delete Quero.dll from system directory
  FileName:=ExpandConstant('{sys}\Quero.dll');
  if FileExists(FileName) then DeleteFile(FileName);

  // Delete uninstallation files from Windows directory
  UninstallString:='';
  if QueroVersion=$03000012 then
    UninstallString:=ExpandConstant('{win}')+'\unins000.exe'
  else if(RegQueryStringValue(HKEY_CURRENT_USER,'Software\Microsoft\Windows\CurrentVersion\Uninstall\Quero Toolbar_is1','UninstallString',UninstallString)=false) then
    RegQueryStringValue(HKEY_LOCAL_MACHINE,'Software\Microsoft\Windows\CurrentVersion\Uninstall\Quero Toolbar_is1','UninstallString',UninstallString);

  if UninstallString<>'' then
  begin
    UninstallString:=RemoveQuotes(UninstallString);
    if FileExists(UninstallString) then DeleteFile(UninstallString);

    FileName:=ExtractFileName(UninstallString);
    StringChange(FileName,'.exe','.dat');
    FileName:=ExtractFilePath(UninstallString)+FileName;

    if FileExists(FileName) then DeleteFile(FileName);
    end;
end;

procedure Remove_x86_Installation();
var
  InstallLocation: String;
begin
  if RegQueryStringValue(HKEY_LOCAL_MACHINE_32,'Software\Microsoft\Windows\CurrentVersion\Uninstall\Quero Toolbar_is1','InstallLocation',InstallLocation) then
  begin
    // Remove x86 installation directory
    InstallLocation:=RemoveQuotes(InstallLocation);
    if DirExists(InstallLocation) then
    begin
      DeleteFile(InstallLocation+'Quero.dll');
      DeleteFile(InstallLocation+'QueroBroker.exe');
      DeleteFile(InstallLocation+'QueroBroker.dll');
      DeleteFile(InstallLocation+'unins000.exe');
      DeleteFile(InstallLocation+'unins000.dat');
      RemoveDir(InstallLocation);
    end;
    // Remove x86 Uninstall entry
    RegDeleteKeyIncludingSubkeys(HKEY_LOCAL_MACHINE_32,'Software\Microsoft\Windows\CurrentVersion\Uninstall\Quero Toolbar_is1');
  end;
end;

procedure CloseInternetExplorer();
var
  ErrorCode: Integer;
begin
  if VISTA_BUILD then
  begin
    ShellExec('open','taskkill.exe','/F /IM iexplore.exe','',SW_HIDE,ewWaitUntilTerminated,ErrorCode);
    ShellExec('open','taskkill.exe','/F /IM QueroBroker.exe','',SW_HIDE,ewWaitUntilTerminated,ErrorCode);
  end
  else
  begin
    ShellExec('open','tskill.exe','iexplore','',SW_HIDE,ewWaitUntilTerminated,ErrorCode);
    ShellExec('open','tskill.exe','QueroBroker','',SW_HIDE,ewWaitUntilTerminated,ErrorCode);
  end;
end;

procedure InitializeWizard();
var
  Page: TWizardPage;
  StaticText: TNewStaticText;
  ProfileId: Cardinal;
  Flags: Cardinal;
  i,y: Integer;

  Reg_QueroVersion: String;
  bVersionLoaded: Boolean;
begin

  // Get Windows Version

  WindowsVersion:=GetWindowsVersion();

  // Determine Quero registry key

  if (WindowsVersion >= $06020000) then Quero_HKCU_Key:=QUERO_HKCU_KEY_INTERNETREGISTRY
  else if (WindowsVersion >= $06000000) then Quero_HKCU_Key:=QUERO_HKCU_KEY_APPDATALOW
  else Quero_HKCU_Key:=QUERO_HKCU_KEY_SOFTWARE;

  // Query installed Quero version

  QueroVersion:=0;
  bTidyUp:=false;

  if VISTA_BUILD then
  begin
    bVersionLoaded:=RegQueryBinaryValue(HKEY_CURRENT_USER,QUERO_HKCU_KEY_INTERNETREGISTRY,'Version',Reg_QueroVersion);
    if (bVersionLoaded=false) then bVersionLoaded:=RegQueryBinaryValue(HKEY_CURRENT_USER,QUERO_HKCU_KEY_APPDATALOW,'Version',Reg_QueroVersion);
  end
  else bVersionLoaded:=RegQueryBinaryValue(HKEY_CURRENT_USER,QUERO_HKCU_KEY,'Version',Reg_QueroVersion);

  if(bVersionLoaded) then
    if(Length(Reg_QueroVersion)>4) then
    begin
      QueroVersion:=Ord(Reg_QueroVersion[4])*$1000000+Ord(Reg_QueroVersion[3])*$10000+Ord(Reg_QueroVersion[2])*$100+Ord(Reg_QueroVersion[1]);
      if(QueroVersion<$03000100) then bTidyUp:=true;
    end;

  bRemove_x86:=(Is64BitInstallMode and RegKeyExists(HKEY_LOCAL_MACHINE_32,'Software\Microsoft\Windows\CurrentVersion\Uninstall\Quero Toolbar_is1'));

  // Determine whether to let the user set the search defaults

  bSetSearchDefaults := not RegKeyExists(HKEY_CURRENT_USER, Quero_HKCU_Key + '\Profiles');

  // Query whether to update search profiles

  if RegQueryDWordValue(HKEY_USERS, '.DEFAULT\Software\Quero Toolbar\Profiles','Flags',Flags) then
    bUpdateProfiles := (Flags and PROFILES_CUSTOM)=0
  else bUpdateProfiles := true;

  // Create custom page

  if bSetSearchDefaults then
  begin

    Page := CreateCustomPage(wpSelectDir, TXT_SEARCH_DEFAULTS, TXT_CHOOSE_SEARCH_DEFAULTS);

    PageID_SearchProfile := Page.ID;

    StaticText := TNewStaticText.Create(Page);
    StaticText.Top :=  0
    StaticText.Caption := TXT_DEFAULT_SEARCH_PROFILE;
    StaticText.AutoSize := True;
    StaticText.Parent := Page.Surface;

    ComboBox_DefaultProfile := TComboBox.Create(Page);
    ComboBox_DefaultProfile.Top := StaticText.Top + StaticText.Height + ScaleY(4);
    //ComboBox_DefaultProfile.Width := Page.SurfaceWidth;
    ComboBox_DefaultProfile.Parent := Page.Surface;
    ComboBox_DefaultProfile.Style := csDropDownList;
    ComboBox_DefaultProfile.Items.Add(TXT_PROFILE_0);
    ComboBox_DefaultProfile.Items.Add(TXT_PROFILE_1);
    ComboBox_DefaultProfile.Items.Add(TXT_PROFILE_2);
    ComboBox_DefaultProfile.Items.Add(TXT_PROFILE_3);
    ComboBox_DefaultProfile.Items.Add(TXT_PROFILE_4);
    ComboBox_DefaultProfile.Items.Add(TXT_PROFILE_5);
    ComboBox_DefaultProfile.Items.Add(TXT_PROFILE_6);
    ComboBox_DefaultProfile.Items.Add(TXT_PROFILE_7);
    ComboBox_DefaultProfile.Items.Add(TXT_PROFILE_8);
    ComboBox_DefaultProfile.Items.Add(TXT_PROFILE_9);
    ComboBox_DefaultProfile.Items.Add(TXT_PROFILE_10);    
    ComboBox_DefaultProfile.ItemIndex := 0;

    StaticText := TNewStaticText.Create(Page);
    StaticText.Top :=  ComboBox_DefaultProfile.Top + ComboBox_DefaultProfile.Height + ScaleY(20);
    StaticText.Caption := TXT_DEFAULT_SEARCH_ENGINE;
    StaticText.AutoSize := True;
    StaticText.Parent := Page.Surface;

    y:=StaticText.Top+StaticText.Height+ScaleY(4);
    i:=0;
    while i<DEFAULT_ENGINES_COUNT do
    begin
      RadioButtons_DefaultEngine[i] := TRadioButton.Create(Page);
      RadioButtons_DefaultEngine[i].Top := y;
      RadioButtons_DefaultEngine[i].Parent := Page.Surface;
      y := y+RadioButtons_DefaultEngine[i].Height+ScaleY(4);
      i := i+1;
    end;

    RadioButtons_DefaultEngine[0].Caption := TXT_GOOGLE;
    RadioButtons_DefaultEngine[1].Caption := TXT_BING;
    RadioButtons_DefaultEngine[2].Caption := TXT_YAHOO;

    StaticText := TNewStaticText.Create(Page);
    StaticText.Top :=  RadioButtons_DefaultEngine[2].Top + RadioButtons_DefaultEngine[2].Height + ScaleY(20);
    StaticText.Caption := TXT_SEARCH_DEFAULTS_NOTE;
    StaticText.AutoSize := True;
    StaticText.Parent := Page.Surface;

  end; // bSetSearchDefaults

end;

function ProfileIndexToId(ProfileIndex: Integer) : Integer;
begin
  case ProfileIndex of
    0: Result:=0;
    1: Result:=1;
    2: Result:=7;
    3: Result:=10;
    4: Result:=4;
    5: Result:=9;
    6: Result:=2;
    7: Result:=6;
    8: Result:=3;
    9: Result:=8;
    10: Result:=5;
  else Result:=0;
  end;
end;

function GetDefaultEngineIndex() : Integer;
var i: Integer;
begin
  i := 0;
  while (i<DEFAULT_ENGINES_COUNT) and (RadioButtons_DefaultEngine[i].Checked = false) do
  begin
    i := i+1;
  end;
  if i=DEFAULT_ENGINES_COUNT then i := 0;

  Result := i;
end;

function UpdateReadyMemo(Space, NewLine, MemoUserInfoInfo, MemoDirInfo, MemoTypeInfo, MemoComponentsInfo, MemoGroupInfo, MemoTasksInfo: String): String;
begin
  Result:=MemoDirInfo;
  if bSetSearchDefaults then
    Result:=Result+NewLine+NewLine+TXT_DEFAULT_SEARCH_PROFILE+NewLine+Space+ComboBox_DefaultProfile.Items[ComboBox_DefaultProfile.ItemIndex]
      +NewLine+NewLine+TXT_DEFAULT_SEARCH_ENGINE+NewLine+Space+RadioButtons_DefaultEngine[GetDefaultEngineIndex()].Caption
  else
  begin
    Result:=Result+NewLine+NewLine+TXT_UPDATE_SEARCH_PROFILE+NewLine+Space;
    if bUpdateProfiles then Result:=Result+TXT_YES
    else Result:=Result+TXT_NO;
  end;
  if bTidyUp or bRemove_x86 then
  begin
    Result:=Result+NewLine+NewLine+TXT_REMOVE_INSTALL;
    if bTidyUp then Result:=Result++NewLine+Space+TXT_TIDY_UP;
    if bRemove_x86 then Result:=Result++NewLine+Space+TXT_X86_INSTALL;
  end;
  Result:=Result+NewLine+NewLine+TXT_CLOSE_IE;
end;

procedure CurStepChanged(CurStep: TSetupStep);
var
  ProfileId: Integer;
  EngineId: Integer;
  i: Integer;
  RegKeyName: String;
  Flags: Cardinal;
begin
  case CurStep of

  ssInstall:
    begin
      CloseInternetExplorer();
      if bTidyUp then TidyUpSysDir();
      if bRemove_x86 then Remove_x86_Installation();
    end;

  ssPostInstall:
    begin
      if bSetSearchDefaults then
      begin

        // Save default profile id

        ProfileId := ProfileIndexToId(ComboBox_DefaultProfile.ItemIndex);

        if ProfileId<>0 then
          RegWriteDWordValue(HKEY_CURRENT_USER,Quero_HKCU_Key+'\Profiles','DefaultId',ProfileId)
        else
          RegDeleteValue(HKEY_CURRENT_USER,Quero_HKCU_Key+'\Profiles','DefaultId');

        // Save default engine id
        
        EngineId := GetDefaultEngineIndex();

        if EngineId<>0 then
        begin
          i := 0;
          while i<PROFILE_COUNT do
          begin
            if i<>PROFILE_ID_SCIENCE then
            begin
              RegKeyName := Quero_HKCU_Key+'\Profiles\'+IntToStr(i);
              RegWriteDWordValue(HKEY_CURRENT_USER,RegKeyName,'DefaultId',EngineId);
              if not RegQueryDWordValue(HKEY_CURRENT_USER,RegKeyName,'Flags',Flags) then Flags := 0;
              Flags := Flags or PROFILE_DEFAULTENGINE_MODIFIED;
              RegWriteDWordValue(HKEY_CURRENT_USER,RegKeyName,'Flags',Flags);
            end;
            i := i+1;
          end;
        end;

      end; // bSetSearchDefaults

      // Under Vista delete the old Quero Toolbar Registry key
      if VISTA_BUILD and (QueroVersion<$04020000) then
      begin
          RegDeleteKeyIncludingSubkeys(HKEY_USERS,'Software\Quero Toolbar');
      end;

    end; // CurStep=ssPostInstall
  end; // End case CurStep
end;

procedure InstallSearchProfiles();
var
  ResultCode: Integer;
begin
  if bUpdateProfiles then
  begin
    RegDeleteKeyIncludingSubkeys(HKEY_USERS,'.DEFAULT\Software\Quero Toolbar\Profiles');
    if not Exec(ExpandConstant('{win}\regedit.exe'),'/s "'+ExpandConstant(CurrentFileName())+'"','',SW_SHOW,ewWaitUntilTerminated,ResultCode) then
      RaiseException('InstallSearchProfiles failed');
  end;
end;

procedure RegisterQueroBroker();
var
  ResultCode: Integer;
begin
  if not Exec(ExpandConstant(CurrentFileName()),'/RegServer','',SW_SHOW,ewWaitUntilTerminated,ResultCode) then
    RaiseException('RegisterQueroBroker failed');
end;

function InitializeUninstall(): Boolean;
begin
  Result:=(MsgBox(TXT_CLOSE_IE, mbConfirmation, MB_OKCANCEL) = IDOK);
end;

procedure CurUninstallStepChanged(CurStep: TUninstallStep); 
begin
  case CurStep of
  usUninstall:
  begin
      CloseInternetExplorer();
  end; // CurSet==usUninstall
  end; // End case CurStep
end;