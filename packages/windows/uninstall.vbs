Option Explicit                 ' Variables must be declared explicitly



'******************************************************************************
' Global constants and variables
'******************************************************************************
dim Tab, NewLine                ' String constants
dim Shell, FSO                  ' Global objects
dim ProgArgs                    ' Program arguments
dim Dbg                         ' Output debugging stuff
dim Language                    ' Program language
dim AppName                     ' Application name
dim Title                       ' Application title
dim UninstallCtrlFileName       ' Name of the uninstall control file
dim SystemDrive                 ' The system drive
dim SystemRoot                  ' The windows directory
dim UserName                    ' Name if the current user
dim UserProfile                 ' User profile directory
dim ProgramFiles                ' Program files directory
dim Failed                      ' Global flag for removal failed
dim RegList                     ' List of registry entries to remove
dim FileList                    ' List of files to remove
dim DirList                     ' List of directories to remove



'******************************************************************************
' Display an error message window with an OK button
'******************************************************************************
sub ErrorMsg (Msg)
    call MsgBox (Msg, vbOkOnly + vbExclamation, Title)
end sub



'******************************************************************************
' Display an error message window and abort the installer
'******************************************************************************
sub Abort (Msg)
    call ErrorMsg (Msg)
    WScript.Quit (1)
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
            case "MSG_ABORT"
                Msg = "Do you want to abort the installation?"
            case "MSG_ADMIN"
                Msg = "You must be Administrator to remove %1."
                Msg = Msg & " Are you sure you want to continue?"
            case "MSG_CTRLFILEERR"
                Msg = "The file %1 is invalid." & NewLine
                Msg = Msg & "Line %2: %3"
            case "MSG_DIRDEL"
                Msg = "Some folders could not be removed:"
                Msg = Msg & NewLine & "%1"
            case "MSG_DUPLICATE"
                Msg = "Duplicate value"
            case "MSG_FAILURE"
                Msg = "Could not remove %1." & NewLine
                Msg = "%2 needs to be run by an Administrator!"
            case "MSG_FILEDEL"
                Msg = "Some files could not be deleted:"
                Msg = Msg & NewLine & "%1"
            case "MSG_OPENERR"
                Msg = "Error opening %1"
            case "MSG_REGDEL"
                Msg = "Some registry entries could not be deleted:"
                Msg = Msg & NewLine & "%1"
            case "MSG_REMOVE"
                Msg = "Remove %1?"
            case "MSG_SUCCESS"
                Msg = "%1 has been successfully removed."
            case "MSG_USAGE"
                Msg = "Usage:" & NewLine & "uninstall appname ctrl-file"
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
' Return an environment string. Fix up Microsoft "innovative" ideas.
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
' Delete a folder and return an error string
'******************************************************************************
function DeleteFolder (Path)
    on error resume next
    call FSO.DeleteFolder (Path, true)
    DeleteFolder = Err.Description
end function



'******************************************************************************
' Delete a file and return an error string
'******************************************************************************
function DeleteFile (Path)
    on error resume next
    call FSO.DeleteFile (Path, true)
    DeleteFile = Err.Description
end function



'******************************************************************************
' Delete a registry entry
'******************************************************************************
function RegDelete (Key)
    on error resume next
    call Shell.RegDelete (Key)
    RegDelete = Err.Description
end function



'******************************************************************************
' Sort an array of strings
'******************************************************************************
sub QS (byref A, Lo, Hi)

    dim I, J, T

    ' Quicksort
    do while Hi > Lo
       	I = Lo + 1
       	J = Hi
       	do while I <= J
       	    do while I <= J
                if StrComp (A(Lo), A(I), vbTextCompare) < 0 then
                    exit do
                end if
       	       	I = I + 1
       	    loop
       	    do while I <= J
                if StrComp (A(Lo), A(J), vbTextCompare) >= 0 then
                    exit do
                end if
       	       	J = J - 1
       	    loop
       	    if I <= J then
                ' Swap A(I) and A(J)
                T = A(I)
                A(I) = A(J)
                A(J) = T
       	     	I = I + 1
       	     	J = J - 1
       	    end if
      	loop
   	if J <> Lo then
            ' Swap A(J) and A(Lo)
            T = A(J)
            A(J) = A(Lo)
            A(Lo) = T
   	end if
       	if (2 * J) > (Hi + Lo) then
 	    call QS (A, J + 1, Hi)
	    Hi = J - 1
	else
	    call QS (A, Lo, J - 1)
    	    Lo = J + 1
	end if
    loop
end sub

sub Quicksort (byref A)
    if UBound (A) > 1 then
        call QS (A, LBound (A), UBound (A))
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
    AppName = ""
    Title = "Uninstaller"
    UninstallCtrlFileName = ""
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
            case else
                if AppName = "" then
                    AppName = ProgArgs(I)
                elseif UninstallCtrlFileName = "" then
                    UninstallCtrlFileName = ProgArgs(I)
                else
                    call ErrorMsg (GetMsg ("MSG_USAGE"))
                    WScript.Quit (1)
                end if
        end select
    next

    ' We need the application name and uninstall control file
    if AppName = "" or UninstallCtrlFileName = "" then
        call Abort (GetMsg ("MSG_USAGE"))
    end if

    ' Set the title early, because it's used in error messages
    Title = AppName & " Uninstaller"

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

    ' Assume we could remove the software
    Failed = false

end sub



'******************************************************************************
' Ask a yes/no question and return the result. "Yes" is default.
'******************************************************************************
function AskYesNo (Question)
    AskYesNo = MsgBox (Question, vbYesNo + vbQuestion + vbDefaultButton1, Title)
end function



'******************************************************************************
' Ask a yes/no question and return the result. "No" is default.
'******************************************************************************
function AskNoYes (Question)
    AskNoYes = MsgBox (Question, vbYesNo + vbQuestion + vbDefaultButton2, Title)
end function



'******************************************************************************
' Ask if the user wants to abort install, and terminate if the answer is yes
'******************************************************************************
sub QueryAbort ()
    if AskNoYes (GetMsg ("MSG_ABORT")) = vbYes then
        WScript.Quit (1)
    end if
end sub



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

    MsgBox Msg, vbOkOnly, "Paths and Locations"
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
' Read the uninstall control file and create the data collections
'******************************************************************************
sub InvalidCtrlFile (Line, Val)
    dim Args(3)
    Args(1) = UninstallCtrlFileName
    Args(2) = Line
    Args(3) = Val
    call Abort (FmtMsg ("MSG_CTRLFILEERR", Args))
end sub

sub ReadUninstallCtrlFile ()

    const ForReading = 1
    dim File, Line, Tag, Args(3)
    dim MyRegList, MyFileList, myDirList

    ' Create some dictionaries. These are not really used as dictionaries, but
    ' have the nice property of expanding dynamically, and we need that.
    set MyRegList  = CreateObject ("Scripting.Dictionary")
    set MyFileList = CreateObject ("Scripting.Dictionary")
    set MyDirList  = CreateObject ("Scripting.Dictionary")

    ' Open the file. Checking Err doesn't work here, don't know why.
    set File = nothing
    on error resume next
    set File = FSO.OpenTextFile (UninstallCtrlFileName, ForReading)
    on error goto 0
    if File is nothing then
        Args(1) = UninstallCtrlFileName
        call Abort (FmtMsg ("MSG_OPENERR", Args))
    end if

    ' Read all lines and insert them in their list
    do while File.AtEndOfStream <> true

        ' Read the next line
        on error resume next
        Line = File.ReadLine
        on error goto 0

        ' Get the type from the line and remove it, so the line contains just
        ' the argument name
        Tag  = Left (Line, 1)
        Line = Mid (Line, 3)

        ' Determine the type of the entry
        select case Tag

            case "D"
                ' A directory. Convert to lowercase to unify names.
                Line = LCase (Line)
                if MyDirList.Exists (Line) then
                    call InvalidCtrlFile (File.Line - 1, GetMsg ("MSG_DUPLICATE"))
                else
                    call MyDirList.Add (Line, "")
                end if

            case "F"
                ' A file. Convert to lowercase to unify names
                Line = LCase (Line)
                if MyFileList.Exists (Line) then
                    call InvalidCtrlFile (File.Line - 1, GetMsg ("MSG_DUPLICATE"))
                else
                    call MyFileList.Add (Line, "")
                end if

            case "R"
                ' A registry entry
                if MyRegList.Exists (Line) then
                    call InvalidCtrlFile (File.Line - 1, GetMsg ("MSG_DUPLICATE"))
                else
                    call MyRegList.Add (Line, "")
                end if

            case else
                call InvalidCtrlFile (File.Line - 1, Tag & " " & Line)

        end select

    loop

    ' Close the file
    on error resume next
    call File.Close ()
    on error goto 0

    ' Make the global arrays big enough for the data
    RegList  = Array (MyRegList.Count)
    FileList = Array (MyFileList.Count)
    DirList  = Array (MyDirList.Count)

    ' Copy the data into the global arrays
    RegList  = MyRegList.Keys
    FileList = MyFileList.Keys
    DirList  = MyDirList.Keys

    ' Sort all the lists. This makes sure nodes are in the array before the
    ' leaves that depend on it. Or in other words: Top level directories and
    ' registry entries come first. So if we delete the items starting at the
    ' other side of the array, we will never delete a subdirectory before its
    ' parent directory.
    call QuickSort (RegList)
    call QuickSort (FileList)
    call QuickSort (DirList)

end sub



'******************************************************************************
' Delete the registry entries
'******************************************************************************
sub DeleteRegistryEntries ()

    dim I, Result, NoDel, Args(1)

    NoDel = ""
    for I = UBound (RegList) to LBound (RegList) step -1
        Result = RegDelete (RegList (I))
        if Result <> "" then
            ' Remember the entries we could not delete
            NoDel = NoDel & RegList (I) & NewLine
        end if
    next

    if NoDel <> "" then
        Args(1) = NoDel
        call ErrorMsg (FmtMsg ("MSG_REGDEL", Args))
    end if
end sub



'******************************************************************************
' Delete the files
'******************************************************************************
sub DeleteFiles ()

    dim I, Result, NoDel, Args(1)

    NoDel = ""
    for I = UBound (FileList) to LBound (FileList) step -1
        Result = DeleteFile (FileList (I))
        if Result <> "" then
            ' Remember the files we could not delete
            NoDel = NoDel & FileList (I) & NewLine
        end if
    next

    if NoDel <> "" then
        Args(1) = NoDel
        call ErrorMsg (FmtMsg ("MSG_FILEDEL", Args))
    end if
end sub



'******************************************************************************
' Delete the directories
'******************************************************************************
sub DeleteDirectories ()

    dim I, Result, NoDel, Args(1)

    NoDel = ""
    for I = UBound (DirList) to LBound (DirList) step -1
        Result = DeleteFolder (DirList (I))
        if Result <> "" then
            ' Remember the directories we could not delete
            NoDel = NoDel & DirList (I) & NewLine
        end if
    next

    if NoDel <> "" then
        Args(1) = NoDel
        call ErrorMsg (FmtMsg ("MSG_DIRDEL", Args))
    end if
end sub



'******************************************************************************
' Function that tells the user that the install was successful
'******************************************************************************
sub Success ()
    dim Args(1), App

    ' Popup message
    Args(1) = AppName
    call MsgBox (FmtMsg ("MSG_SUCCESS", Args), vbOkOnly + vbInformation, Title)

end sub



'******************************************************************************
' Function that tells the user that the uninstall failed
'******************************************************************************
sub Failure ()
    dim Args(2)

    ' Popup message
    Args(1) = AppName
    Args(2) = Title
    ErrorMsg (FmtMsg ("MSG_FAILURE", Args))
    WScript.Quit (1)

end sub



'******************************************************************************
' Main program
'******************************************************************************
sub Main ()

    dim Args(1)

    ' Initialize global variables. This includes the paths used
    InitializeGlobals ()
    if Dbg then
        ShowPathsAndLocations ()
    end if

    ' Check that we're running this script as admin
    CheckAdminRights ()

    ' Let the user make up his mind
    Args(1) = AppName
    if AskYesNo (FmtMsg ("MSG_REMOVE", Args)) <> vbYes then
        WScript.Quit (1)
    end if

    ' Read the uninstall control file
    call ReadUninstallCtrlFile ()

    ' Delete the registry entries
    call DeleteRegistryEntries ()

    ' Delete all files
    call DeleteFiles ()

    ' Delete the directories
    call DeleteDirectories ()

    ' We're done
    if Failed then
        Failure ()
    else
        Success ()
    end if
end sub



'******************************************************************************
' The script body just calls Main...
'******************************************************************************
Main ()




