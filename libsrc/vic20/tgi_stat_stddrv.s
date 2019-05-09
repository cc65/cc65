;
; const void tgi_static_stddrv[];
;

        .export _tgi_static_stddrv
        .import _vic20_hi_tgi

.rodata

_tgi_static_stddrv := _vic20_hi_tgi
