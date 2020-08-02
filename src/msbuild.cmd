call "%VS140COMNTOOLS%vsvars32.bat"
set VCTargetsPath=%VCInstallDir%\VCTargets\
set
msbuild.exe %*
