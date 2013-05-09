;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: boolean negation for longs
;

        .export         bnegeax
        .importzp       sreg, tmp1

bnegeax:
        stx     tmp1
        ldx     #0              ; High byte of result
        ora     tmp1
        ora     sreg
        ora     sreg+1
        bne     @L0
        lda     #1
        rts

@L0:    txa                     ; X is zero
        rts

