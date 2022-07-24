;
; Address of the static standard tgi driver
;
; Jede (jede@oric.org), 2017-10-15
;
; const void tgi_static_stddrv[];
;

        .export _tgi_static_stddrv
        .import _telestrat_240_200_2_tgi

.rodata

_tgi_static_stddrv := _telestrat_240_200_2_tgi
