;
; Address of the static standard mouse driver
;
; Oliver Schmidt, 2012-11-01
;
; const void mouse_static_stddrv[];
;

        .export	_mouse_static_stddrv
        .import	_c128_1351

.rodata

_mouse_static_stddrv := _c128_1351
