;
; Ullrich von Bassewitz, 06.08.1998
;
; unsigned char kbhit (void);
;

        .export         _kbhit
        .import         keyidx: zp


.proc   _kbhit

        ldx     #0              ; High byte of return is always zero
        lda     keyidx          ; Get number of characters
        beq     L9
        lda     #1
L9:     rts

.endproc





