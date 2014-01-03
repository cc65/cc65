;
; Name of the standard mouse driver
;
; Christian Groessler, 2014-01-02
;
; const char mouse_stddrv[];
;

        .export _mouse_stddrv

.rodata

_mouse_stddrv:
        .ifdef  __ATARIXL__
        .asciiz "ATRXJOY.MOU"
        .else
        .asciiz "ATRJOY.MOU"
        .endif
