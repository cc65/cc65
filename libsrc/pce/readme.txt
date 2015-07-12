PC-Engine (PCE) target support for cc65. this is still work in progress and
a couple of things need to be fixed:
--------------------------------------------------------------------------------

joystick support should get verified on real hw
 - the masks for buttons may be wrong.
 - 6 button gamepads are different and need slightly different code

revers() is a dummy function, actual reverse output is not supported yet

waitvblank() is missing
get_tv() is missing

some graphical petscii chars should get added to the charset

conio-init should get initialized from a constructor rather than always get
called from crt0 (which for some reason doesnt work) -> see conio.s, it should
get linked if _any_ of the conio function is used

interruptor support in crt0 (and cfg) is missing

--------------------------------------------------------------------------------

a good emulator to use for PC-Engine is "mednafen" (mednafen.sourceforge.net)

run the compiled binary like this:

> mednafen -force_module pce <yourprogram.pce>

joypad keys are mapped like this:

w/s/a/d         up/down/left/right
numpad 2        (?) button
numpad 3        (?) button
enter           (start) button

--------------------------------------------------------------------------------
some useful resources on PCE coding:

http://blog.blockos.org/?tag=pc-engine
http://pcedev.blockos.org/viewforum.php?f=5
http://www.romhacking.net/?page=documents&category=&platform=4&game=&author=&perpage=20&level=&title=&desc=&docsearch=Go
http://archaicpixels.com/Main_Page

http://www.magicengine.com/mkit/doc.html

https://github.com/uli/huc
http://www.zeograd.com/parse.php?src=hucf