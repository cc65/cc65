;
; Address of the static standard joystick driver
;
; Oliver Schmidt, 2012-11-01
; Robert Justice, 2026
;
; const void joy_static_stddrv[];
;

        .export _joy_static_stddrv
        .import _a3_stdjoy_joy

.rodata

_joy_static_stddrv := _a3_stdjoy_joy
