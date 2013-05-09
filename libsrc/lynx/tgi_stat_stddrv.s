;
; Address of the static standard tgi driver
;
; Oliver Schmidt, 2012-11-01
;
; const void tgi_static_stddrv[];
;

        .export _tgi_static_stddrv
        .import _lynx_160_102_16_tgi

.rodata

_tgi_static_stddrv := _lynx_160_102_16_tgi
