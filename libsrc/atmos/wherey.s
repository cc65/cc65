;
; Ullrich von Bassewitz, 2003-04-13
;
; unsigned char wherey (void);
;

        .export         _wherey

        .include        "atmos.inc"

.proc   _wherey

        ldx     #$00
        lda     CURS_Y
        rts

.endproc


