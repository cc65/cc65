;
; Address of the static standard joystick driver
;
; Oliver Schmidt, 2012-11-01
;
; const void joy_static_stddrv[];
;

        .export         _joy_static_stddrv
        .import         _pce_stdjoy_joy

.rodata

_joy_static_stddrv      := _pce_stdjoy_joy
