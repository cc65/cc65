;
; Name of the standard joystick driver
;
; Ullrich von Bassewitz, 2002-12-21
;
; const char joy_stddrv[];
;

            .export _joy_stddrv

.rodata

_joy_stddrv:
        .asciiz "geos-stdjoy.joy"

