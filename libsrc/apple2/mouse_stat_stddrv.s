;
; Address of the static standard mouse driver
;
; Oliver Schmidt, 2012-11-01
;
; const void mouse_static_stddrv[];
;

        .export	_mouse_static_stddrv
        .import	_apple2_stdmou

.rodata

_mouse_static_stddrv := _apple2_stdmou
