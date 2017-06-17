
        .export         _waitvblank

        .include        "c128.inc"

_waitvblank:
@l1:
        lda     VIC_CTRL1
        bpl     @l1
@l2:
        lda     VIC_CTRL1
        bmi     @l2
        rts
