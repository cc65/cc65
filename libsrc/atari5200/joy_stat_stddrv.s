;
; Address of the static standard joystick driver
;
; Christian Groessler, 2014-05-12
;
; const void joy_static_stddrv[];
;

        .export _joy_static_stddrv
        .import _atr5200std_joy

_joy_static_stddrv := _atr5200std_joy
