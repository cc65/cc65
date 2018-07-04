;
; Ullrich von Bassewitz, 21.09.1998
;
; clock_t clock (void);
;

        .include        "cbm.inc"

        .export         _clock
        .importzp       sreg


.proc   _clock

        lda     #0              ; Byte 3 is always zero
        sta     sreg+1
        jsr     RDTIM
        sty     sreg
        rts

.endproc
