call "%VS140COMNTOOLS%vsvars32.bat"
set VCTargetsPath=%VCIDEInstallDir%\VCTargets\
msbuild.exe %*
