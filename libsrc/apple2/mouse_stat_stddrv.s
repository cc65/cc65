;
; Address of the static standard mouse driver
;
; Oliver Schmidt, 2012-11-01
;
; const void mouse_static_stddrv[];
;

        .export _mouse_static_stddrv
        .ifdef  __APPLE2ENH__
        .import _a2e_stdmou_mou
        .else
        .import _a2_stdmou_mou
        .endif

.rodata

        .ifdef  __APPLE2ENH__
_mouse_static_stddrv := _a2e_stdmou_mou
        .else
_mouse_static_stddrv := _a2_stdmou_mou
        .endif
