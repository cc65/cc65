;
; Ullrich von Bassewitz, 2003-05-02
;
; unsigned char wherex (void);
;

        .export         _wherex

        .include        "gamate.inc"
        .include        "extzp.inc"

.proc   _wherex

        lda     CURS_X
        ldx     #$00
        rts

.endproc


