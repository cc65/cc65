;
; Ullrich von Bassewitz, 06.08.1998
;
; unsigned char wherex (void);
;

        .export         _wherex
        .import         CURS_X: zp


.proc   _wherex
        lda     CURS_X
        ldx     #$00
        rts
.endproc

