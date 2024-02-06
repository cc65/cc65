;
; Ullrich von Bassewitz, 05.08.1998
;
; CC65 runtime: boolean negation
;

        .export         bnega, bnegax
        .import         return0, return1

bnegax: cpx     #0
        bne     L0
bnega:  tax
        bne     L0
L1:     lda     #1              ; Zero already in X
        rts

L0:     ldx     #0
        txa
        rts

