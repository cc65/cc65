;
; Ullrich von Bassewitz, 06.08.1998
; Copied from cbm/wherey.s
;
; unsigned char wherey (void);
;
        .export         _wherey
        .import         CURS_Y: zp

.proc   _wherey
        lda     CURS_Y
        ldx     #$00
        rts
.endproc
