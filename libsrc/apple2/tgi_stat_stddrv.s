;
; Address of the static standard tgi driver
;
; Oliver Schmidt, 2012-11-01
;
; const void tgi_static_stddrv[];
;

        .export _tgi_static_stddrv
        .ifdef  __APPLE2ENH__
        .import _a2e_hi_tgi
        .else
        .import _a2_hi_tgi
        .endif

.rodata

        .ifdef  __APPLE2ENH__
_tgi_static_stddrv := _a2e_hi_tgi
        .else
_tgi_static_stddrv := _a2_hi_tgi
        .endif
