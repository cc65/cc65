;
; Ullrich von Bassewitz, 2003-05-02
;
; unsigned char wherex (void);
;

        .export         _wherex

        .include        "nes.inc"

.proc   _wherex

        lda     CURS_X
        ldx     #$00
        rts

.endproc


