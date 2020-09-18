;
; acqn, 01.16.2020
;
; CC65 runtime: boolean cast for longs
;

        .export         bcasteax
        .importzp       sreg, tmp1

bcasteax:
        stx     tmp1
        ldx     #0              ; High byte of result
        ora     tmp1
        ora     sreg
        ora     sreg+1
        beq     L0

        lda     #1
L0:     rts

