
        .export         _waitvblank

        .include        "c128/c128.inc"

_waitvblank:
        lda     PALFLAG
        beq     @ntsc
        ldx     #(312-24)-256
        .byte $2c
@ntsc:
        ldx     #(262-4)-256
@l1:
        lda     VIC_CTRL1
        and     #$80
        beq     @l1
@l2:
        cpx     VIC_HLINE
        bcs     @l2
        rts
