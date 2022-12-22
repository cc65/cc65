;
; Address of the static standard serial driver
;
; Oliver Schmidt, 2022-12-22
;
; const void ser_static_stddrv[];
;

        .export _ser_static_stddrv
        .import _lynx_comlynx_ser

.rodata

_ser_static_stddrv := _lynx_comlynx_ser
