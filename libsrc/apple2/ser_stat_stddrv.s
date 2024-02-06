;
; Address of the static standard serial driver
;
; Oliver Schmidt, 2022-12-22
;
; const void ser_static_stddrv[];
;

        .export _ser_static_stddrv
        .ifdef  __APPLE2ENH__
        .import _a2e_ssc_ser
        .else
        .import _a2_ssc_ser
        .endif

.rodata

        .ifdef  __APPLE2ENH__
_ser_static_stddrv := _a2e_ssc_ser
        .else
_ser_static_stddrv := _a2_ssc_ser
        .endif
