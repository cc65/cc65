;
; Name of the standard mouse driver
;
; 2009-09-11, Ullrich von Bassewitz
; 2013-06-25, Greg King
;
; const char mouse_stddrv[];
;

        .export _mouse_stddrv

.rodata

_mouse_stddrv:  .asciiz "cbm510-joy.mou"


