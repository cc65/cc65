;
; Ullrich von Bassewitz, 2003-05-02
;
; unsigned char wherex (void);
;

        .export         _wherex
        .include        "creativision.inc"

.proc   _wherex

        lda     CURSOR_X
        ldx     #$00
        rts

.endproc
