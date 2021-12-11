;
; Address of the static standard TGI driver
;
; 2020-06-04, Greg King
;
; const void tgi_static_stddrv[];
;

        .import _cx320p1_tgi    ; 320 pixels across, 1 pixel per byte

        .export _tgi_static_stddrv := _cx320p1_tgi
