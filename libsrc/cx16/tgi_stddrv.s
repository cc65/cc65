;
; Name of the standard TGI driver
;
; 2019-12-22, Greg King
;
; const char tgi_stddrv[];
;

        .export _tgi_stddrv

.rodata
_tgi_stddrv:    .asciiz "cx16-320x8b.tgi"
