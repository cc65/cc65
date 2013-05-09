;
; Ullrich von Bassewitz, 05.10.1998
;
; CC65 runtime: Make boolean according to flags
;

        .export         boolne, booleq, boollt, boolle, boolgt, boolge
        .export         boolult, boolule, boolugt, booluge


boolne: bne     ret1
        ldx     #$00
        txa
        rts


booleq: beq     ret1
        ldx     #$00
        txa
        rts


boolle: beq     ret1
boollt: bmi     ret1
        ldx     #$00
        txa
        rts


boolgt: beq     L0
boolge: bpl     ret1
L0:     ldx     #$00
        txa
        rts


boolule:
        beq     ret1
boolult:
        bcc     ret1
        ldx     #$00
        txa
        rts


boolugt:
        beq     L1
booluge:
        bcs     ret1
L1:     ldx     #$00
        txa
        rts


ret1:   ldx     #$00
        lda     #$01
        rts



