;
; Address of the static standard joystick driver
;
; Christian Groessler, 2017-02-06
;
; const void joy_static_stddrv[];
;

        .export _joy_static_stddrv
        .import _creativisionstd_joy

_joy_static_stddrv := _creativisionstd_joy
