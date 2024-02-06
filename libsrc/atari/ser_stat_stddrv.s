;
; Address of the static standard serial driver
;
; Oliver Schmidt, 2022-12-22
;
; const void ser_static_stddrv[];
;

        .export _ser_static_stddrv
        .ifdef  __ATARIXL__
        .import _atrxrdev_ser
        .else
        .import _atrrdev_ser
        .endif

.rodata

        .ifdef  __ATARIXL__
_ser_static_stddrv := _atrxrdev_ser
        .else
_ser_static_stddrv := _atrrdev_ser
        .endif
