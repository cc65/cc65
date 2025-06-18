;
; Address of the static standard tgi driver
;
; Oliver Schmidt, 2012-11-01
;
; const void tgi_static_stddrv[];
;

        .export _tgi_static_stddrv
        .import _ted_hi_tgi

.rodata

_tgi_static_stddrv := _ted_hi_tgi
