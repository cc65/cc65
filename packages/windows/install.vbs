Option Explicit                 ' Variables must be declared explicitly



'******************************************************************************
' Installer defaults.
'******************************************************************************
const AppName     = "cc65"
const Version     = "2.10.5.20050325"
const Installer   = "cc65 Installer"
const SpaceNeeded = 20                  ' Free space needed on drive in MB
const Shortcut    = true                ' Create shortcut on desktop



'******************************************************************************
' Global constants
'******************************************************************************
const SysEnv  = "HKLM\System\CurrentControlSet\Control\Session Manager\Environment"
const SysPath = "HKLM\System\CurrentControlSet\Control\Session Manager\Environment\Path"



'******************************************************************************
' Global variables
'******************************************************************************
dim Tab, NewLine                ' String constants
dim Shell, FSO                  ' Global objects
dim ProgArgs                    ' Program arguments
dim Dbg                         ' Output debugging stuff
dim Language                    ' Program language
dim SystemDrive                 ' The system drive
dim SystemRoot                  ' The windows directory
dim UserName                    ' Name if the current user
dim UserProfile                 ' User profile directory
dim ProgramFiles                ' Program files directory
dim AppData                     ' Application data directory
dim InstallSource               ' Installation source directory
dim InstallTarget               ' Installation target directory
dim UninstallCtrlFileName       ' Name of the control file for the uninstaller
dim UninstallCtrlFile           ' Control file for the uninstaller
dim Uninstaller                 ' Path to the uninstaller file
dim UninstallerCmdLine          ' Command line for the uninstaller
dim Programs                    ' "Programs" menu folder
dim Desktop                     ' "Desktop" menu folder
dim RegUninstall                ' Registry key for uninstall entries
dim BinDir			' Directory for binaries
dim LibDir			' Library directory
dim IncDir			' Include directory
dim DocIndexFile	    	' Name of documentation index file
dim AnnouncementFile		' Name of the announcement file



'******************************************************************************
' Display an error message window with an OK button
'******************************************************************************
sub ErrorMsg (Msg)
    call MsgBox (Msg, vbOkOnly + vbExclamation, Installer)
end sub



'******************************************************************************
' Display an error message window and abort the installer
'******************************************************************************
sub Abort (Msg)
    call ErrorMsg (Msg)
    WScript.Quit (1)
end sub



'******************************************************************************
' Display a message with an OK button
'******************************************************************************
sub Message (Msg)
    call MsgBox (Msg, vbOkOnly + vbInformation, Installer)
end sub



'******************************************************************************
' Convert a number to a string
'******************************************************************************
function ToString (Num)
    ToString = FormatNumber (Num, vbFalse, vbTrue, vbFalse, vbFalse)
end function



'******************************************************************************
' Return a message in the current language
'******************************************************************************
function GetMsg (Key)
    dim Msg

    ' Handle other languages here

    ' Default is english
    if IsEmpty (Msg) then
        ' No assignment, use english
        select case Key
            case "MSG_ABORTINFO"
                Msg = "Installation was aborted."
            case "MSG_ADMIN"
                Msg = "You must be Administrator to install %1."
                Msg = Msg & " Are you sure you want to continue?"
            case "MSG_COPYERR"
                Msg = "Cannot copy %1 to %2: " & NewLine & "%3"
            case "MSG_CREATEDIR"
                Msg = "%1 does not exist." & NewLine & "Create it?"
            case "MSG_CREATEERR"
                Msg = "Cannot create %1:" & NewLine & "%2"
            case "MSG_DELETEERR"
                Msg = "Cannot delete %1:" & NewLine & "%2"
            case "MSG_DRIVESPACE"
                Msg = "Not enough space left on drive %1" & NewLine
                Msg = Msg & "At least %2 MB are needed."
            case "MSG_INSTALLPATH"
                Msg = "The package will be installed in %1"
            case "MSG_DOCENTRY"
                Msg = "%1 Documentation"
            case "MSG_REGREADERR"
                Msg = "Installation failed: Cannot read the registry!"
            case "MSG_REGWRITEERR"
                Msg = "Installation failed: Cannot write to the registry!"
            case "MSG_REMOVEENTRY"
                Msg = "Remove %1"
            case "MSG_REMOVEDIR"
                Msg = "A folder with the name %1 does already exist."
                Msg = Msg & " Is it ok to remove the folder?"
            case "MSG_REMOVEOLD"
                Msg = "Found an old version. Remove it?"
            case "MSG_SUCCESS"
                Msg = "Installation was successful!"
            case "MSG_UNINSTALLERR"
                Msg = "There was a problem uninstalling the old version. Please"
                Msg = Msg & " uninstall the old program manually and restart"
                Msg = Msg & " the installation."
            case "MSG_ANNOUNCEMENT"
                Msg = "cc65 Announcement"
            case "MSG_INCOMPLETE"
                Msg = "The package seems to be incomplete and cannot be"
                Msg = Msg & " installed."
            case else
                Msg = Key
        end select
    end if
    GetMsg = Msg
end function



'******************************************************************************
' Format a string replacing %n specifiers in the format string F
'******************************************************************************
function Fmt (F, Values)
    dim I, Count, Key, Val, Start, Pos
    Count = UBound (Values)     ' How many values?
    for I = Count to 0 step -1
        Key = "%" & ToString (I)
        select case VarType (Values (I))
            case vbEmpty
                Val = ""
            case vbInteger
                Val = ToString (Values (I))
            case vbLong
                Val = ToString (Values (I))
            case vbNull
                Val = ""
            case vbSingle
                Val = ToString (Values (I))
            case vbDouble
                Val = ToString (Values (I))
            case vbString
                Val = Values (I)
            case else
                Abort ("Internal error: Invalid conversion in Format()")
        end select
        F = Replace (F, Key, Val)
    next
    Fmt = F
end function



'******************************************************************************
' Format a message replacing %n specifiers in the format string F
'******************************************************************************
function FmtMsg (Msg, Values)
    FmtMsg = Fmt (GetMsg (Msg), Values)
end function



'******************************************************************************
' Return an environment string. Fix up Microsofts "innovative" ideas.
'******************************************************************************
function GetEnv (Key)
    dim Value
    Value = Shell.ExpandEnvironmentStrings (Key)
    if Value = Key then
        GetEnv = vbNullString
    else
        GetEnv = Value
    end if
end function



'******************************************************************************
' Build a path from two components
'******************************************************************************
function BuildPath (Path, Name)
    BuildPath = FSO.BuildPath (Path, Name)
end function



'******************************************************************************
' Return true if the file with the given name exists
'******************************************************************************
function FileExists (Name)
    On Error Resume Next
    FileExists = FSO.FileExists (Name)
end function



'******************************************************************************
' Return true if the folder with the given name exists
'******************************************************************************
function FolderExists (Name)
    On Error Resume Next
    FolderExists = FSO.FolderExists (Name)
end function



'******************************************************************************
' Copy a file and return an error message (empty string if no error)
'******************************************************************************
function CopyFile (Source, Target)
    if Right (Target, 1) <> "\" and FolderExists (Target) then
        Target = Target & "\"
    end if
    On Error Resume Next
    call FSO.CopyFile (Source, Target)
    on error goto 0
    CopyFile = Err.Description
end function



'******************************************************************************
' Create a folder and all parent folders and return an error string
'******************************************************************************
function CreateFolder (Path)

    ' If the parent folder does not exist, try to create it
    dim ParentFolder
    ParentFolder = FSO.GetParentFolderName (Path)
    if ParentFolder <> "" and not FolderExists (ParentFolder) then
        CreateFolder (ParentFolder)
    end if

    ' Now try to create the actual folder
    On Error Resume Next
    FSO.CreateFolder (Path)
    CreateFolder = Err.Description

end function



'******************************************************************************
' Delete a file and return an error string
'******************************************************************************
function DeleteFile (Name)
    On Error Resume Next
    call FSO.DeleteFile (Name, true)
    DeleteFile = Err.Description
end function



'******************************************************************************
' Delete a folder and return an error string
'******************************************************************************
function DeleteFolder (Path)
    On Error Resume Next
    call FSO.DeleteFolder (Path, true)
    DeleteFolder = Err.Description
end function



'******************************************************************************
' Return the type of a registry entry
'******************************************************************************
function RegType (Value)

    dim Result

    ' Determine the type of the registry value. If the string contains percent
    ' signs, use REG_EXPAND_SZ, otherwise use REG_SZ. This isn't always safe,
    ' but there is no way to determine the type, and VBS itself is too stupid
    ' to choose the correct type itself. Add the usual curse over Microsoft
    ' here...
    Result = InStr (1, Value, "%")
    if Result = 0 then
	RegType = "REG_SZ"
    else
	RegType = "REG_EXPAND_SZ"
    end if
end function



'******************************************************************************
' Read a string from the registry. Return an empty string if nothing was found.
'******************************************************************************
function RegReadStr (Key)
    On Error Resume Next
    RegReadStr = Shell.RegRead (Key)
    if Err.Number <> 0 then
        RegReadStr = ""
    end if
end function



'******************************************************************************
' Write a binary value to the registry, return an error description
'******************************************************************************
function RegWriteBin (Key, Value)
    on error resume next
    Shell.RegWrite Key, Value, "REG_BINARY"
    RegWriteBin = Err.Description
    on error goto 0
    WriteUninstallCtrlFile ("R " & Key)
end function



'******************************************************************************
' Write a string value to the registry, return an error description
'******************************************************************************
function RegWriteStr (Key, Value)
    on error resume next
    Shell.RegWrite Key, Value, "REG_SZ"
    RegWriteStr = Err.Description
    on error goto 0
    WriteUninstallCtrlFile ("R " & Key)
end function



'******************************************************************************
' Run a program, wait for its termination and return an error code.
'******************************************************************************
function Run (Cmd, WinState)
    dim ErrCode

    On Error Resume Next
    ErrCode = Shell.Run (Cmd, WinState, true)
    if Err.Number <> 0 then
        ErrCode = Err.Number
    end if
    Run = ErrCode
end function



'******************************************************************************
' Display a progress bar using the internet exploder
'******************************************************************************
dim PBDoc               ' Progress bar document object
dim PBVal               ' Current progress bar setting
dim IEApp               ' Internet exploder application object
set PBDoc = nothing
set IEApp = nothing
PBVal = -1

sub ProgressBar (Percent)

    ' Remember the last setting
    PBVal = Percent

    'Create the progress bar window
    if PBDoc is nothing then

        if ((Cint (Percent) < 0) or (Cint (Percent) > 100)) then
            ' Close request, but object already destroyed
            exit sub
        end if

        ' Create an object that control the internet exploder
        set IEApp = CreateObject ("InternetExplorer.Application")

        ' Set the exploder to fullscreen and retrieve its size
        dim ScreenHeight, ScreenWidth
        IEApp.Visible    = false
        IEApp.FullScreen = true
        ScreenWidth      = IEApp.Width
        ScreenHeight     = IEApp.Height
        IEApp.FullScreen = false

        ' Now prepare the actual window
        IEApp.Offline    = true
        IEApp.AddressBar = false
        IEApp.Height     = 100
        IEApp.Width      = 250
        IEApp.MenuBar    = false
        IEApp.StatusBar  = false
        IEApp.Silent     = true
        IEApp.ToolBar    = false
        IEApp.Resizable  = false
        IEApp.Left       = (ScreenWidth  - IEApp.Width)  / 2
        IEApp.Top        = (ScreenHeight - IEApp.Height) / 2
        call IEApp.Navigate ("about:blank")
        do while IEApp.Busy
            call WScript.Sleep (100)
        loop

        ' Connect to the displayed document
        do until not PBDoc is nothing
            call WScript.Sleep (100)
            set PBDoc = IEApp.Document
        loop

        ' Generate a new document showing a progress bar
        PBDoc.Open
        call PBDoc.Write ("<html><head><title>" & Installer & " progress</title></head>")
        call PBDoc.Write ("<body bgcolor=#C0C0C0><center>")
        call PBDoc.Write ("<table width=100% border=1 frame=box><tr><td>")
        call PBDoc.Write ("<table id=progress width=0 border=0 cellpadding=0 cellspacing=0 bgcolor=#FFFFFF>")
        call PBDoc.Write ("<tr><td>&nbsp</td></tr></table>")
        call PBDoc.Write ("</td></tr></table>")
        call PBDoc.Write ("</center></body></html>")
        PBDoc.Close

        ' Display the exploder window
        IEApp.Visible = true

    else
        if ((Cint (Percent) < 0) or (Cint (Percent) > 100)) then
            ' Request for shutdown
            IEApp.Visible = false
            set PBDoc = nothing
            IEApp.Quit
            set IEApp = nothing
        else
            ' Update the progress bar
            if Cint (Percent) = 0 then
                PBDoc.all.progress.width = "1%"
                PBDoc.all.progress.bgcolor = "#C0C0C0"
            else
                PBDoc.all.progress.width = Cstr (Cint (Percent)) & "%"
                PBDoc.all.progress.bgcolor = "#0000C0"
            end if
        end if
    end if
end sub



'******************************************************************************
' Initialize global variables
'******************************************************************************
sub InitializeGlobals ()
    dim I

    ' String stuff used for formatting
    Tab     = Chr (9)
    NewLine = Chr (13)

    ' Global objects
    set Shell = WScript.CreateObject ("WScript.Shell")
    set FSO   = CreateObject ("Scripting.FileSystemObject")

    ' Arguments
    set ProgArgs = WScript.Arguments

    ' Handle program arguments
    Dbg = false
    Language = "de"
    for I = 0 to ProgArgs.Count-1
        select case ProgArgs(I)
            case "-de"
                Language = "de"
            case "-debug"
                Dbg = true
            case "-en"
                Language = "en"
        end select
    next

    ' Paths and locations
    SystemDrive = GetEnv ("%SystemDrive%")
    if SystemDrive = vbNullString then
        SystemDrive = "c:"
    end if
    SystemRoot = GetEnv ("%SystemRoot%")
    if SystemRoot = vbNullString then
        SystemRoot = BuildPath (SystemDrive, "winnt")
    end if
    UserName = GetEnv ("%USERNAME%")
    if UserName = vbNullString then
        UserName = "Administrator"
    end if
    UserProfile = GetEnv ("%USERPROFILE%")
    if UserProfile = vbNullString then
        UserProfile = BuildPath (SystemDrive, "Dokumente und Einstellungen\" & UserName)
    end if
    ProgramFiles = GetEnv ("%ProgramFiles%")
    if ProgramFiles = vbNullString then
        ProgramFiles = BuildPath (SystemDrive, "Programme")
    end if
    AppData = GetEnv ("%AppData%")
    if AppData = vbNullString then
        AppData = UserProfile
    end if
    InstallSource = FSO.GetParentFolderName (WScript.ScriptFullName)
    InstallTarget = BuildPath (ProgramFiles, AppName)

    Programs = Shell.SpecialFolders ("AllUsersPrograms")
    Desktop  = Shell.SpecialFolders ("AllUsersDesktop")

    ' Uninstaller
    set UninstallCtrlFile = nothing
    Uninstaller = BuildPath (InstallTarget, "uninstall.vbs")
    UninstallCtrlFileName = BuildPath (InstallTarget, "uninstall.lst")
    UninstallerCmdLine = "-" & Language & " " & AppName & " " & UninstallCtrlFileName

    ' Registry paths
    RegUninstall = "HKLM\Software\Microsoft\Windows\CurrentVersion\Uninstall\" & AppName & "\"

    ' Directories
    BinDir = BuildPath (InstallTarget, "bin")
    LibDir = BuildPath (InstallTarget, "lib")
    IncDir = BuildPath (InstallTarget, "include")

    ' Files
    AnnouncementFile = "announce.txt"
    DocIndexFile     = "doc\index.html"
end sub



'******************************************************************************
' Ask a yes/no question and return the result. "Yes" is default.
'******************************************************************************
function AskYesNo (Question)
    AskYesNo = MsgBox (Question, vbYesNo + vbQuestion + vbDefaultButton1, Installer)
end function



'******************************************************************************
' Ask a yes/no question and return the result. "No" is default.
'******************************************************************************
function AskNoYes (Question)
    AskNoYes = MsgBox (Question, vbYesNo + vbQuestion + vbDefaultButton2, Installer)
end function



'******************************************************************************
' Tell the user that the installation was aborted and terminate the script
'******************************************************************************
sub InfoAbort ()
    call MsgBox (GetMsg ("MSG_ABORTINFO"), vbOkOnly + vbInformation, Installer)
    WScript.Quit (0)
end sub



'******************************************************************************
' Input routine with the window caption preset
'******************************************************************************
function Input (Prompt, Default)
    Input = InputBox (Prompt, Installer, Default)
end function



'******************************************************************************
' Check if a directory is a given the path
'******************************************************************************
function DirInPath (ByVal Dir)

    dim Path, Entries, I

    ' Get the path in lower case
    Path = LCase (GetEnv ("%Path%"))

    ' Convert the directory to lower case
    Dir = LCase (Dir)

    ' Split the path into separate entries
    Entries = Split (Path, ";")

    ' Check all entries
    for I = LBound (Entries) to UBound (Entries)
	if Entries(I) = Dir then
	    DirInPath = true
	    exit function
	end if
    next

    DirInPath = false
end function




'******************************************************************************
' Function that displays the paths and locations found
'******************************************************************************
function OneLoc (Key, Value)
    dim Result
    Result = Trim (Key)
    if Len (Result) <= 8 then
        Result = Result & Tab
    end if
    OneLoc = Result & Tab & "=" & Tab & Value & NewLine
end function

sub ShowPathsAndLocations ()
    dim Msg
    Msg = Msg & OneLoc ("SystemDrive",   SystemDrive)
    Msg = Msg & OneLoc ("SystemRoot",    SystemRoot)
    Msg = Msg & OneLoc ("UserName",      UserName)
    Msg = Msg & OneLoc ("UserProfile",   UserProfile)
    Msg = Msg & OneLoc ("ProgramFiles",  ProgramFiles)
    Msg = Msg & OneLoc ("AppData",       AppData)
    Msg = Msg & OneLoc ("InstallSource", InstallSource)
    Msg = Msg & OneLoc ("InstallTarget", InstallTarget)
    Msg = Msg & OneLoc ("Programs",      Programs)
    Msg = Msg & OneLoc ("Desktop",       Desktop)
    Msg = Msg & OneLoc ("Free space",    ToString (GetDriveSpace (InstallTarget)))

    call MsgBox (Msg, vbOkOnly, "Paths and Locations")
end sub



'******************************************************************************
' Return the amount of free space for a path (in Megabytes)
'******************************************************************************
function GetDriveSpace (Path)
    dim Drive
    On Error Resume Next
    set Drive = FSO.GetDrive (FSO.GetDriveName (Path))
    if Err.Number <> 0 then
        GetDriveSpace = 0
    else
        GetDriveSpace = Drive.FreeSpace / (1024 * 1024)
    end if
end function



'******************************************************************************
' Check that there's something to install
'******************************************************************************
sub CheckFilesToInstall ()

    ' If the uninstaller is unavailable for some reason or the other, we
    ' have a problem, because the installer will create an uninstaller entry
    ' in the registry, but it will not work, which means that the package
    ' cannot be deinstalled or overwritten. So we have to check that at least
    ' the uninstaller is available in the same directory as the installer.
    if not FileExists (Uninstaller) then
        Abort (GetMsg ("MSG_INCOMPLETE"))
    end if
end sub



'******************************************************************************
' Check that were running this script as admin
'******************************************************************************
sub CheckAdminRights ()

    ' FIXME: This check is not perfect
    if UserName <> "Administrator" then
        dim Args(1)
        Args(1) = AppName

        if AskNoYes (FmtMsg ("MSG_ADMIN", Args)) <> vbYes then
            WScript.Quit (1)
        end if
    end if

end sub



'******************************************************************************
' Remove an old installation.
'******************************************************************************
sub RemoveOldInstall (UninstallCmd)

    dim ErrCode

    ' Execute the uninstall
    ErrCode = Run (UninstallCmd, 0)

    ' Tell the user that the uninstall is done
    if ErrCode <> 0 then
        call Abort (GetMsg ("MSG_UNINSTALLERR"))
    end if
end sub



'******************************************************************************
' Check if there is an old installation. Offer to remove it.
'******************************************************************************
sub CheckOldInstall ()

    dim UninstallCmd

    ' Read the uninstall command from the registry
    UninstallCmd = RegReadStr (RegUninstall & "UninstallString")

    ' Check if there is already an executable
    if UninstallCmd <> "" then

        ' Ask to remove an old install
        if AskYesNo (GetMsg ("MSG_REMOVEOLD")) = vbYes then
            ' Remove the old installation
            call RemoveOldInstall (UninstallCmd)
        end if

    end if

end sub



'******************************************************************************
' Check that the install target exists. Offer to create it.
'******************************************************************************
sub CheckInstallTarget ()

    dim Msg, Result, Args(2)

    ' Tell the user about the install target and ask if it's ok
    Args(1) = InstallTarget
    Msg = FmtMsg ("MSG_INSTALLPATH", Args)
    if MsgBox (Msg, vbOkCancel, Installer) <> vbOk then
        call InfoAbort ()
    end if

    ' Check if there's enough space left on the target drive
    if GetDriveSpace (InstallTarget) < SpaceNeeded then
        Args(1) = FSO.GetDriveName (InstallTarget)
        Args(2) = SpaceNeeded
        call Abort (FmtMsg ("MSG_DRIVESPACE", Args))
    end if

    ' Check if the install path exist, create it if necessary
    if not FolderExists (InstallTarget) then
        Result = CreateFolder (InstallTarget)
        if Result <> "" then
            Args(1) = InstallTarget
            Args(2) = Result
            call Abort (FmtMsg ("MSG_CREATEERR", Args))
        end if
    end if

end sub



'******************************************************************************
' Create the uninstall control file
'******************************************************************************
sub CreateUninstallCtrlFile ()

    dim Filename

    ' Generate the filename
    on Error resume next
    set UninstallCtrlFile = FSO.CreateTextFile (UninstallCtrlFileName, true)
    on error goto 0
    if Err.Number <> 0 then
        dim Args(2)
        Args(1) = UninstallCtrlFileName
        Args(2) = Err.Description
        call ErrorMsg (FmtMsg ("MSG_CREATEERR", Args))
        WScript.Quit (1)
    end if

    ' Write the name of the target directory to the file
    call WriteUninstallCtrlFile ("D " & InstallTarget)

    ' Write the name of the file itself to the file
    call WriteUninstallCtrlFile ("F " & UninstallCtrlFileName)

end sub



'******************************************************************************
' Write to the uninstall control file
'******************************************************************************
sub WriteUninstallCtrlFile (Line)

    on error resume next
    UninstallCtrlFile.WriteLine (Line)
    if Err.Number <> 0 then
        dim Args(2)
        Args(1) = UninstallCtrlFileName
        Args(2) = Err.Description
        call ErrorMsg (FmtMsg ("MSG_WRITEERR", Args))
        WScript.Quit (1)
    end if

end sub



'******************************************************************************
' Close the uninstall control file
'******************************************************************************
sub CloseUninstallCtrlFile ()

    on error resume next
    UninstallCtrlFile.Close
    if Err.Number <> 0 then
        dim Args(2)
        Args(1) = UninstallCtrlFileName
        Args(2) = Err.Description
        call ErrorMsg (FmtMsg ("MSG_WRITEERR", Args))
        WScript.Quit (1)
    end if

end sub



'******************************************************************************
' Copy the application files
'******************************************************************************
sub RecursiveCopy (Dir, SourcePath, TargetPath)

    dim File, TargetFile, SubDir, SourceName, TargetName, Result, Args(3)

    ' Copy all files in this folder
    for each File in Dir.Files

        ' Generate source and target file names
        SourceName = BuildPath (SourcePath, File.Name)
        TargetName = BuildPath (TargetPath, File.Name)

        ' Copy the file. The error check doesn't seem to work.
        on error resume next
        File.Copy (TargetName)
        on error goto 0
        if Err.Number <> 0 then
            Args(1) = SourceName
            Args(2) = TargetName
            Args(3) = Err.Description
            call ErrorMsg (FmtMsg ("MSG_COPYERR", Args))
            call AbortInfo ()
        end if

        ' Remove the r/o attribute from the target file if set
        set TargetFile = FSO.GetFile (TargetName)
        if TargetFile.Attributes mod 2 = 1 then
            TargetFile.Attributes = TargetFile.Attributes - 1
        end if

        ' Remember this file in the uninstall control file
        call WriteUninstallCtrlFile ("F " & TargetName)
    next

    ' Handle all subdirectories
    for each SubDir in Dir.SubFolders

        ' Update the progress bar with each copied directory
        if PBVal <= 80 then
            call ProgressBar (PBVal + 5)
        end if

        ' Generate the new directory names
        SourceName = BuildPath (SourcePath, SubDir.Name)
        TargetName = BuildPath (TargetPath, SubDir.Name)

        ' Generate the new target dir. Notify the user about errors, but
        ' otherwise ignore them.
        Result = CreateFolder (TargetName)
        if Result <> "" then
            ' Display an error but try to continue
            Args(1) = TargetName
            Args(2) = Result
            call ErrorMsg (FmtMsg ("MSG_CREATEERR", Args))
        end if

        ' Recursively process files in the subdirectory
        call RecursiveCopy (SubDir, SourceName, TargetName)

        ' Remember the subdirectory in the uninstall control file
        WriteUninstallCtrlFile ("D " & TargetName)

    next
end sub

sub CopyFiles ()

    ' Update the progress bar
    call ProgressBar (10)

    ' Copy all files generating entries in the uninstall control file
    call RecursiveCopy (FSO.GetFolder (InstallSource), InstallSource, InstallTarget)

    ' Update the progress bar
    call ProgressBar (90)
end sub



'******************************************************************************
' Create the registry entries
'******************************************************************************
sub CreateRegEntries ()

    dim Cmd

    ' Create the entry in Systemsteuerung -> Software. Check if the first write
    ' succeeds. If not, we don't have admin rights.
    if RegWriteBin (RegUninstall, 1) <> "" then
        call Abort (GetMsg ("MSG_REGWRITEERR"))
    end if
    call RegWriteStr (RegUninstall & "DisplayName", AppName & " " & Version)
    call RegWriteStr (RegUninstall & "UninstallString", "wscript //nologo " & Uninstaller & " " & UninstallerCmdLine)

end sub



'******************************************************************************
' Function that creates an URL
'******************************************************************************
sub CreateUrl (Name, Url, Description)
    ' Ignore errors
    On Error Resume Next

    dim Link
    set Link = Shell.CreateShortcut (Name)
    Link.TargetPath = Url
    Link.Description = Description
    Link.Save

    ' Allow errors again
    on error goto 0

    ' Write the file name to the uninstall control file
    WriteUninstallCtrlFile ("F " & Name)
end sub



'******************************************************************************
' Function that creates a shortcut
'******************************************************************************
sub CreateShortcut (Name, Exe, Args, Description)
    ' Ignore errors
    On Error Resume Next

    dim Link
    set Link = Shell.CreateShortcut (Name)
    Link.TargetPath  = Exe
    Link.Arguments   = Args
    Link.WindowStyle = 1
    Link.Description = Description
    Link.WorkingDirectory = AppData
    Link.Save

    ' Allow errors again
    on error goto 0

    ' Write the file name to the uninstall control file
    WriteUninstallCtrlFile ("F " & Name)
end sub



'******************************************************************************
' Function that creates the menu entries
'******************************************************************************
sub CreateMenuEntries ()
    dim Folder, Result, Name, Desc, Target, Args(2)

    ' Create the start menu folder.
    Folder = BuildPath (Programs, AppName)
    Result = CreateFolder (Folder)
    if Result <> "" then
        ' Display an error but try to continue
        Args(1) = Folder
        Args(2) = Result
        call ErrorMsg (FmtMsg ("MSG_CREATEERR", Args))
    end if

    ' Create an uninstall shortcut in the menu folder
    Args(1) = AppName
    Desc = FmtMsg ("MSG_REMOVEENTRY", Args)
    Name = BuildPath (Folder, Desc & ".lnk")
    call CreateShortcut (Name, Uninstaller, UninstallerCmdLine, Desc)

    ' Create a documentation shortcut in the menu folder
    Target = BuildPath (InstallTarget, DocIndexFile)
    if FileExists (Target) then
        Args(1) = AppName
        Desc = FmtMsg ("MSG_DOCENTRY", Args)
        Name = BuildPath (Folder, Desc & ".url")
        call CreateUrl (Name, "file://" & Target, Desc)
    end if

    ' Create the shortcut to the announcement in the menu folder
    Target = BuildPath (InstallTarget, AnnouncementFile)
    if FileExists (Target) then
        Desc = GetMsg ("MSG_ANNOUNCEMENT")
        Name = BuildPath (Folder, Desc & ".url")
        call CreateUrl (Name, "file://" & Target, Desc)
    end if

    ' Update the uninstall control file
    call WriteUninstallCtrlFile ("D " & Folder)
end sub



'******************************************************************************
' Add a directory to the system path
'******************************************************************************
sub AddToSysPath (Dir)

    dim Path

    ' Handle errors. Assume failure
    on error resume next

    ' Retrieve the PATH setting
    Path = Shell.RegRead (SysPath)
    if Err.Number <> 0 then
    	' Could not read
        call Abort (GetMsg ("MSG_REGREADERR"))
    end if

    ' Add the new directory to the path
    if (Len (Path) > 0) and (Right (Path, 1) <> ";") then
    	Path = Path + ";"
    end if
    Path = Path + Dir

    ' Write the new path
    call Shell.RegWrite (SysPath, Path, "REG_EXPAND_SZ")
    if Err.Number <> 0 then
    	' Could not set
        call Abort (GetMsg ("MSG_REGWRITEERR"))
    end if
end sub



'******************************************************************************
' Add environment variables
'******************************************************************************
sub AddEnvironment ()

    ' Add CC65_LIB
    if RegWriteStr (SysEnv & "\CC65_LIB", LibDir) <> "" then
        call Abort (GetMsg ("MSG_REGWRITEERR"))
    end if

    ' Add CC65_INC
    if RegWriteStr (SysEnv & "\CC65_INC", IncDir) <> "" then
        call Abort (GetMsg ("MSG_REGWRITEERR"))
    end if

    ' Add the bin directory to the path if it's not already there
    if not DirInPath (BinDir) then
	call AddToSysPath (BinDir)

	' Run the wm_envchange program to notify other running programs
	' of the changed environment. Ignore errors.
	call Run (BuildPath (BinDir, "wm_envchange.exe"), 0)
    end if

end sub



'******************************************************************************
' Function that tells the user that the install was successful
'******************************************************************************
sub Success ()
    call MsgBox (GetMsg ("MSG_SUCCESS"), vbOkOnly + vbInformation, Installer)
end sub



'******************************************************************************
' Main program
'******************************************************************************
sub Main ()

    ' Initialize global variables. This includes the paths used
    call InitializeGlobals ()
    if Dbg then
        call ShowPathsAndLocations ()
    end if

    ' Check that there's something to install
    call CheckFilesToInstall ()

    ' Check that we're running this script as admin
    call CheckAdminRights ()

    ' Check if there is an old installation and offer to remove it
    call CheckOldInstall ()

    ' Check if the source directory does really exist
    call CheckInstallTarget ()

    ' Display the progress bar
    call ProgressBar (0)

    ' Create the uninstall file
    call CreateUninstallCtrlFile ()
    call ProgressBar (2)

    ' Create registry entries
    CreateRegEntries ()
    call Progressbar (5)

    ' Copy the application files (will do automatic progress bar updates)
    call CopyFiles ()

    ' Create the menu entries
    call CreateMenuEntries ()
    call ProgressBar (90)

    ' Add entries to the enviroment
    call AddEnvironment ()
    call ProgressBar (95)

    ' Close the uninstall control file
    call CloseUninstallCtrlFile ()

    ' We're done
    call ProgressBar (100)
    call ProgressBar (-1)
    call Success ()

    ' Return a success code
    WScript.Quit (0)
end sub



'******************************************************************************
' The script body just calls Main...
'******************************************************************************
Main ()





