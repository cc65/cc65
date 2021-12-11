;
; Name of the standard TGI driver
;
; 2018-03-11, Sven Michael Klose
;
; const char tgi_stddrv[];
;

        .export _tgi_stddrv

.rodata

_tgi_stddrv:    .asciiz "vic20-hi.tgi"
