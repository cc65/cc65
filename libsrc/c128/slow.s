;
; Ullrich von Bassewitz, 2003-02-13
;
; void slow (void);
; /* Switch the CPU into 1MHz mode. */
;  */
;

        .export         _slow

        .include        "c128.inc"


.proc   _slow

        lda     #$00
        sta     VIC_CLK_128
        rts

.endproc


