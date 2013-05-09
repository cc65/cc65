;
; Ullrich von Bassewitz, 06.08.1998
;
; unsigned char kbhit (void);
;

        .export         _kbhit

        .include        "plus4.inc"


.proc   _kbhit

        ldx     #0              ; High byte of return is always zero
        lda     KEY_COUNT       ; Get number of characters
        ora     FKEY_COUNT      ; Or with number of chars from function keys
        beq     L9
        lda     #1
L9:     rts

.endproc



