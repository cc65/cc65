;
; Address of the static standard TGI driver
;
; const void tgi_static_stddrv[];
;

        .import _vic20_hi_tgi
        .export _tgi_static_stddrv := _vic20_hi_tgi
