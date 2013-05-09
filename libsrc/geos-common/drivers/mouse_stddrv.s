;
; Name of the standard mouse driver
;
; 2010-01-25, Greg King
;
; const char mouse_stddrv[];
;

            .export _mouse_stddrv

.rodata

_mouse_stddrv:
        .asciiz "geos-stdmou.mou"
