;
; Address of the static standard mouse driver
;
; 2019-11-08, Greg King
;
; const void mouse_static_stddrv[];
;

        .import _cx16_std_mou

        .export _mouse_static_stddrv := _cx16_std_mou
