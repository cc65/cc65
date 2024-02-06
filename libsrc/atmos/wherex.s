;
; Ullrich von Bassewitz, 2003-04-13
;
; unsigned char wherex (void);
;

        .export         _wherex

        .include        "atmos.inc"

.proc   _wherex

        ldx     #$00
        lda     CURS_X
        rts

.endproc

