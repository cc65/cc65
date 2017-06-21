
        .export         _waitvblank

        .include        "c64.inc"

_waitvblank:
@l1:
        bit     VIC_CTRL1
        bpl     @l1
@l2:
        bit     VIC_CTRL1
        bmi     @l2
        rts
