;
; Address of the static standard tgi driver
;
; Oliver Schmidt, 2012-11-01
;
; const void tgi_static_stddrv[];
;

        .export _tgi_static_stddrv
        .ifdef  __ATARIXL__
        .import _atrx8_tgi
        .else
        .import _atr8_tgi
        .endif

.rodata

        .ifdef  __ATARIXL__
_tgi_static_stddrv := _atrx8_tgi
        .else
_tgi_static_stddrv := _atr8_tgi
        .endif
