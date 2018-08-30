;
; Name of the standard tgi driver
;
; Stefan Haubenthal, 2018-08-29
;
; const char tgi_stddrv[];
;

        .export _tgi_stddrv

.rodata

_tgi_stddrv:    .asciiz "pet-lo.tgi"
