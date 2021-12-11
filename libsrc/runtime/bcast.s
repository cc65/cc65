;
; acqn, 01.16.2020
;
; CC65 runtime: boolean cast
;

        .export         bcasta, bcastax

bcastax:
        cpx     #0
        bne     L1

bcasta:
        tax
        beq     L0              ; Zero already in X

L1:     ldx     #0
        lda     #1

L0:     rts

