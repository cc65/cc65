;
; Name of the standard joystick driver
;
; Oliver Schmidt, 2012-11-01
;
; const char joy_stddrv[];
;

        .export         _joy_stddrv

.rodata

_joy_stddrv:    .asciiz "gamate-stdjoy.joy"
