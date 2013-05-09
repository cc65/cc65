;
; Name of the standard tgi driver
;
; Oliver Schmidt, 2012-11-01
;
; const char tgi_stddrv[];
;

        .export _tgi_stddrv

.rodata

_tgi_stddrv:    .asciiz "lynx-160-102-16.tgi"
