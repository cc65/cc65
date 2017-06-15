;
; Ullrich von Bassewitz, 2003-05-02
;
; unsigned char wherey (void);
;

        .export         _wherey

        .include        "gamate.inc"
        .include        "extzp.inc"

.proc   _wherey

        lda     CURS_Y
        ldx     #$00
        rts

.endproc


