;
; Address of the static standard TGI driver
;
; 2019-12-22, Greg King
;
; const void tgi_static_stddrv[];
;

        .import _cx16_320x8b_tgi
        .export _tgi_static_stddrv := _cx16_320x8b_tgi
