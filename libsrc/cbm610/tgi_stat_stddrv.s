;
; Address of the static standard tgi driver
;
; Stefan Haubenthal, 2018-08-29
;
; const void tgi_static_stddrv[];
;

        .export _tgi_static_stddrv
        .import _cbm610_lo_tgi

.rodata

_tgi_static_stddrv := _cbm610_lo_tgi
