;
; Address of the static standard joystick driver
;
; Oliver Schmidt, 2012-11-01
;
; const void joy_static_stddrv[];
;

        .export _joy_static_stddrv
        .ifdef  __ATARIXL__
        .import _atrxstd_joy
        .else
        .import _atrstd_joy
        .endif

.rodata

        .ifdef  __ATARIXL__
_joy_static_stddrv := _atrxstd_joy
        .else
_joy_static_stddrv := _atrstd_joy
        .endif
