@echo off

set CC65VER=2.9.0

REM Cleanup old stuff
rm -r cc65

REM Create the directories
mkdir cc65 > nul
mkdir cc65\bin > nul

REM Copy stuff from trixie
for %i in (asminc doc emd include lib tgi) do (
    if not exist u:\cc65\%i (
     	echo u:\cc65\%i does not exist
     	quit 1
    )
    mkdir cc65\%i
    xcopy /q /s u:\cc65\%i\* cc65\%i
)

move /q cc65\doc\announce.txt cc65
ren /q cc65\doc\samples cc65\samples

REM Check if the compiler source exists
if not exist c:\uz\src\cc65 (
    echo c:\uz\src\cc65 does not exist
    quit 1
)

for %i in (apple2 atari c16 c64 c128 cbm510 cbm610 geos pet plus4 vic20) do (
    setlocal
    cd cc65
    if exist u:\cc65\cc65-%i-%CC65VER%.zip del /q u:\cc65\cc65-%i-%CC65VER%.zip
    zip -q9m u:\cc65\cc65-%i-%CC65VER%.zip lib\%i.lib lib\%i.o include\%i.h doc\%i.cfg
    if exist emd\%i*.emd zip -q9m u:\cc65\cc65-%i-%CC65VER%.zip emd\%i*.emd
    if exist tgi\%i*.tgi zip -q9m u:\cc65\cc65-%i-%CC65VER%.zip tgi\%i*.tgi
    endlocal
)

REM Specials for the GEOS package
cd cc65
zip -q9mr u:\cc65\cc65-geos-%CC65VER%.zip include\geos samples\geos
cd ..

REM Generate the win32 package
(
    setlocal
    cd src\cc65\src
    gmake -f make\watcom.mak clean
    gmake -f make\watcom.mak
    gmake -f make\watcom.mak strip
    endlocal
)

del /q /y cc65\bin\*
copy /q src\cc65\src\ar65\ar65.exe cc65\bin
copy /q src\cc65\src\ca65\ca65.exe cc65\bin
copy /q src\cc65\src\cc65\cc65.exe cc65\bin
copy /q src\cc65\src\cl65\cl65.exe cc65\bin
copy /q src\cc65\src\da65\da65.exe cc65\bin
copy /q src\cc65\src\grc\grc.exe cc65\bin
copy /q src\cc65\src\ld65\ld65.exe cc65\bin
copy /q src\cc65\src\od65\od65.exe cc65\bin
if exist u:\cc65\cc65-win32-%CC65VER%.zip del /q u:\cc65\cc65-win32-%CC65VER%.zip
cd cc65
zip -q9r u:\cc65\cc65-win32-%CC65VER%.zip *
cd ..

REM Generate the os2 package
(
    setlocal
    cd src\cc65\src
    gmake -f make\watcom.mak clean
    gmake -f make\watcom.mak os2
    gmake -f make\watcom.mak strip
    endlocal
)

del /q /y cc65\bin\*
copy /q src\cc65\src\ar65\ar65.exe cc65\bin
copy /q src\cc65\src\ca65\ca65.exe cc65\bin
copy /q src\cc65\src\cc65\cc65.exe cc65\bin
copy /q src\cc65\src\cl65\cl65.exe cc65\bin
copy /q src\cc65\src\da65\da65.exe cc65\bin
copy /q src\cc65\src\grc\grc.exe cc65\bin
copy /q src\cc65\src\ld65\ld65.exe cc65\bin
copy /q src\cc65\src\od65\od65.exe cc65\bin
if exist u:\cc65\cc65-os2-%CC65VER%.zip del /q u:\cc65\cc65-os2-%CC65VER%.zip
cd cc65
zip -q9r u:\cc65\cc65-os2-%CC65VER%.zip *
cd ..

REM Generate the dos32 package
(
    setlocal
    cd src\cc65\src
    gmake -f make\watcom.mak clean
    gmake -f make\watcom.mak dos
    gmake -f make\watcom.mak strip
    endlocal
)

del /q /y cc65\bin\*
copy /q src\cc65\src\ar65\ar65.exe cc65\bin
copy /q src\cc65\src\ca65\ca65.exe cc65\bin
copy /q src\cc65\src\cc65\cc65.exe cc65\bin
copy /q src\cc65\src\cl65\cl65.exe cc65\bin
copy /q src\cc65\src\da65\da65.exe cc65\bin
copy /q src\cc65\src\grc\grc.exe cc65\bin
copy /q src\cc65\src\ld65\ld65.exe cc65\bin
copy /q src\cc65\src\od65\od65.exe cc65\bin
copy /q \watcom\binw\dos4gw.exe cc65\bin
if exist u:\cc65\cc65-dos32-%CC65VER%.zip del /q u:\cc65\cc65-dos32-%CC65VER%.zip
cd cc65
zip -q9r u:\cc65\cc65-dos32-%CC65VER%.zip *
cd ..



