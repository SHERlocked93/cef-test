; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "nj-multiday"
#define MyAppVersion "1.0.6"
#define MyAppPublisher "panda"
#define MyAppExeName "nj-multiday.exe"
#define MyAppId "{92D28235-3413-4C04-8C4A-CFD700FBB810}"

[Setup]
; NOTE: The value of AppId uniquely identifies this application. Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{#MyAppId}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName}{#MyAppVersion}
AppPublisher={#MyAppPublisher}
DefaultDirName=c:\{#MyAppName}
DisableProgramGroupPage=yes
; Uncomment the following line to run in non administrative install mode (install for current user only.)
;PrivilegesRequired=lowest
OutputDir=C:\Users\qiany\Downloads
OutputBaseFilename={#MyAppName}{#MyAppVersion}-setup
Compression=lzma
SolidCompression=yes
WizardStyle=modern

[Languages]
//Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "chinese"; MessagesFile: "compiler:Languages\ChineseSimplified.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "C:\PandaProjects\Codes\MultiDay\nj-multiday-qt\bin32\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\PandaProjects\Codes\MultiDay\nj-multiday-qt\bin32\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{autoprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
//Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall runascurrentuser

[Code]
//旧版本检测，安装新版本
function GetNumber(var temp: String): Integer;
var
    part: String;
    pos1: Integer;
begin
    if Length(temp) = 0 then
    begin
        Result := -1;
        Exit;
    end;
        pos1 := Pos('.', temp);
        if (pos1 = 0) then
        begin
            Result := StrToInt(temp);
            temp := '';
        end
        else
        begin
            part := Copy(temp, 1, pos1 - 1);
            temp := Copy(temp, pos1 + 1, Length(temp));
            Result := StrToInt(part);
        end;
end;

function CompareInner(var temp1, temp2: String): Integer;
var
    num1, num2: Integer;
begin
    num1 := GetNumber(temp1);
    num2 := GetNumber(temp2);
    if (num1 = -1) or (num2 = -1) then
    begin
        Result := 0;
        Exit;
    end;
        if (num1 > num2) then
        begin
            Result := 1;
        end
        else if (num1 < num2) then
        begin
            Result := -1;
        end
        else
        begin
            Result := CompareInner(temp1, temp2);
        end;
end;

function CompareVersion(str1, str2: String): Integer;
var
    temp1, temp2: String;
begin
    temp1 := str1;
    temp2 := str2;
    Result := CompareInner(temp1, temp2);
end;

function UpdateSetup(): Boolean;
var
    oldVersion: String;
    uninstaller: String;
    ErrorCode: Integer;
begin

    if RegKeyExists(HKEY_LOCAL_MACHINE,'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{#MyAppId}_is1') then
    begin
        RegQueryStringValue(HKEY_LOCAL_MACHINE,'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{#MyAppId}_is1','DisplayVersion', oldVersion);
        if (CompareVersion(oldVersion, '{#MyAppVersion}') < 0) then
        begin
        if MsgBox('已安装版本：v' + oldVersion + ' 的应用. 是否覆盖安装新版本?',mbConfirmation, MB_YESNO) = IDYES then
        begin
            Result := True;
        end
        else
        begin
            Result := False;
        end;
        end
        else
        begin
        MsgBox('已安装较新或相同版本 v' + oldVersion + ' 的应用。安装程序即将退出',mbInformation, MB_OK);
        Result := False;
        end;
    end
    else
    begin
        Result := True;
    end;
end;


procedure CloseApp(AppName: String);
var
    WbemLocator : Variant;
    WMIService   : Variant;
    WbemObjectSet: Variant;
    WbemObject   : Variant;
begin;
    WbemLocator := CreateOleObject('WbemScripting.SWbemLocator');
    WMIService := WbemLocator.ConnectServer('localhost', 'root\CIMV2');
    WbemObjectSet := WMIService.ExecQuery('SELECT * FROM Win32_Process Where Name="' + AppName + '"');
    if not VarIsNull(WbemObjectSet) and (WbemObjectSet.Count > 0) then
    begin
        WbemObject := WbemObjectSet.ItemIndex(0);
        if not VarIsNull(WbemObject) then
        begin
        WbemObject.Terminate();
        WbemObject := Unassigned;
        end;
    end;
end;

function InitializeSetup(): Boolean;
var
    Success:Boolean;
    IsRunning: Integer;
begin
    Success:=true;

    IsRunning:=FindWindowByWindowName('{#MyAppName}');
    if IsRunning<>0 then
    begin
        if Msgbox('安装程序检测到客户端正在运行。' #13#13 '您必须先关闭它然后单击“是”继续安装，或按“否”退出！', mbConfirmation, MB_YESNO) = idNO then
        begin
            Success:=false;
        end
        else
        begin
            CloseApp('{#MyAppExeName}');
        end;
    end
    else begin end;

    if Success then
    begin
        //升级检测
        if not UpdateSetup() then
        begin
            Success:=false;
        end
        else begin end;
    end;

    result := Success;
end;
