;
; 2022-04-16, Karri Kaksonen
;
; unsigned char wherey()
;

        .export         _wherey
        .include        "extzp.inc"

;-----------------------------------------------------------------------------
; Get cursor Y position
;
        .proc   _wherey

        ldx     #0
        lda     CURS_Y
        rts
        .endproc

