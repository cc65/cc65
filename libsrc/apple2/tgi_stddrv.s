;
; Name of the standard tgi driver
;
; Oliver Schmidt, 2011-05-02
;
; const char tgi_stddrv[];
;

        .export _tgi_stddrv

.rodata

_tgi_stddrv:
        .ifdef  __APPLE2ENH__
        .asciiz "A2E.HI.TGI"
        .else
        .asciiz "A2.HI.TGI"
        .endif
