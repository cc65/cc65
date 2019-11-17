;
; Name of the standard TGI driver
;
; 2019-11-06, Greg King
;
; const char tgi_stddrv[];
;

        .export _tgi_stddrv

.rodata

_tgi_stddrv:    .asciiz "cx16-640x4c.tgi"
