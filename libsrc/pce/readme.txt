
joystick support should get verified on real hw, the masks for buttons may be
wrong.

clock() does not work for unknown reasons

get_tv() is a dummy function and always returns 0

revers() is a dummy function, actual reverse output is not supported yet

waitvblank() is missing

some graphical petscii chars should get added to the charset

conio-init should get initialized from a constructor rather than always get
called from crt0

--------------------------------------------------------------------------------

a good emulator to use for PC-Engine is "mednafen" (mednafen.sourceforge.net)

run the compiled binary like this:

> mednafen -force_module pce <yourprogram.pce>

joypad keys are mapped like this:

w/s/a/d         up/down/left/right
numpad 2        (?) button
numpad 3        (?) button
enter           (start) button
