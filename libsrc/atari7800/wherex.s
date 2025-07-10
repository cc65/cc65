;
; 2022-04-16, Karri Kaksonen
;
; unsigned char wherex()
;

        .export         _wherex
        .include        "extzp.inc"

;-----------------------------------------------------------------------------
; Get cursor X position
;
        .proc   _wherex

        ldx     #0
        lda     CURS_X
        rts
        .endproc

