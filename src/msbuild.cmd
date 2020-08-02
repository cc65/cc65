call "%VS140COMNTOOLS%vsvars32.bat"
where /R "c:\Program Files (x86)" VsDevCmd.bat
set
msbuild.exe %*
