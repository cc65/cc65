;
; Address of the static standard mouse driver
;
; 2012-11-01, Oliver Schmidt
; 2013-08-06, Greg King
;
; const void mouse_static_stddrv[];
;

        .export _mouse_static_stddrv
        .import _cbm510_joy_mou

_mouse_static_stddrv := _cbm510_joy_mou
