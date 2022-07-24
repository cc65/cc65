;
; Ullrich von Bassewitz, 2003-05-02
;
; unsigned char wherey (void);
;

        .export         _wherey
        .include        "creativision.inc"

.proc   _wherey

        lda     CURSOR_Y
        ldx     #$00
        rts

.endproc
