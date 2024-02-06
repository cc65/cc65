;
; Address of the static standard joystick driver
;
; Oliver Schmidt, 2012-11-01
;
; const void joy_static_stddrv[];
;

        .export _joy_static_stddrv
        .ifdef  __APPLE2ENH__
        .import _a2e_stdjoy_joy
        .else
        .import _a2_stdjoy_joy
        .endif

.rodata

        .ifdef  __APPLE2ENH__
_joy_static_stddrv := _a2e_stdjoy_joy
        .else
_joy_static_stddrv := _a2_stdjoy_joy
        .endif
