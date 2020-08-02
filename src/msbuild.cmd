call "%VS140COMNTOOLS%vsvars32.bat"
where /R c:\ Microsoft.Cpp.targets
set VCTargetsPath=%VCInstallDir%\VCTargets\
set
msbuild.exe %*
