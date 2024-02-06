;
; Name of the standard joystick driver
;
; 2019-11-10, Greg King
;
; const char joy_stddrv[];
;

        .export _joy_stddrv

.rodata

_joy_stddrv:    .asciiz "cx16-std.joy"
