        .export         _waitvblank

        .include        "vic20/vic20.inc"

_waitvblank:
        lda     PALFLAG
        beq     @ntsc
        ldx     #(312-8)/2
        .byte $2c
@ntsc:
        ldx     #(262-8)/2
@l2:
        cpx     VIC_HLINE
        bcs     @l2
        rts

