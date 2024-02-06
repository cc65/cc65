;
; Name of the standard mouse driver
;
; 2019-11-08, Greg King
;
; const char mouse_stddrv[];
;

        .export _mouse_stddrv

.rodata

_mouse_stddrv:  .asciiz "cx16-std.mou"
