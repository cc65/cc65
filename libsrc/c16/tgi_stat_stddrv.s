;
; Address of the static standard tgi driver
;
; Stefan Haubenthal, 2018-08-29
;
; const void tgi_static_stddrv[];
;

        .export _tgi_static_stddrv
        .import _c16_lo_tgi

.rodata

_tgi_static_stddrv := _c16_lo_tgi
