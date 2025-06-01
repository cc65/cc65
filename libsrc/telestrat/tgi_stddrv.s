;
; Name of the standard TGI driver
;
; 2022-02-02, Greg King
;
; const char tgi_stddrv[];
;

        .export _tgi_stddrv

.rodata

_tgi_stddrv:    .asciiz "telestrat-240-200-2.tgi"
