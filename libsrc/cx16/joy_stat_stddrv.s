;
; Address of the static standard joystick driver
;
; 2019-11-10, Greg King
;
; const void joy_static_stddrv[];
;

        .import _cx16_std_joy

        .export _joy_static_stddrv := _cx16_std_joy
