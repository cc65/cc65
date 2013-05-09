;
; Name of the standard mouse driver
;
; Ullrich von Bassewitz, 2009-09-11
;
; const char mouse_stddrv[];
;

        .export _mouse_stddrv

.rodata

_mouse_stddrv:
        .ifdef  __APPLE2ENH__
        .asciiz "A2E.STDMOU.MOU"
        .else
        .asciiz "A2.STDMOU.MOU"
        .endif
