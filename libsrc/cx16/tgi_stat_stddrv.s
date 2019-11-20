;
; Address of the static standard TGI driver
;
; 2019-11-06, Greg King
;
; const void tgi_static_stddrv[];
;

        .import _cx16_640x4c_tgi

        .export _tgi_static_stddrv := _cx16_640x4c_tgi
