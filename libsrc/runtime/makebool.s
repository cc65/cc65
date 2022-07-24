;
; Ullrich von Bassewitz, 05.10.1998
; Christian Krueger, 11-Mar-2017, optimization
;
; CC65 runtime: Make boolean according to flags
;

        .export         boolne, booleq, boollt, boolle, boolgt, boolge
        .export         boolult, boolule, boolugt, booluge


boolne: bne     ret1
ret0:   ldx     #$00
        txa
        rts


booleq: bne     ret0
ret1:   ldx     #$00
        lda     #$01
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
        beq     ret0
booluge:
        ldx     #$00
        txa
        rol     a
        rts
