;
; Address of the static standard joystick driver
;
; Oliver Schmidt, 2012-11-01
;
; const void joy_static_stddrv[];
;

        .export         _joy_static_stddrv
        .import         _gamate_stdjoy_joy

_joy_static_stddrv      := _gamate_stdjoy_joy
