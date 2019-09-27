;
; Address of the static standard joystick driver
;
; 2019-09-19, Greg King
;
; const void joy_static_stddrv[];
;

        .import _cx16_stdjoy_joy
        .export _joy_static_stddrv := _cx16_stdjoy_joy
