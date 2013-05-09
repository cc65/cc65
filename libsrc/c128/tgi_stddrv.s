;
; Name of the standard tgi driver
;
; Oliver Schmidt, 2011-05-02
;
; const char tgi_stddrv[];
;

        .export _tgi_stddrv

.rodata

_tgi_stddrv:    .asciiz "c128-vdc.tgi"
