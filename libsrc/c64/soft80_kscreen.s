
        .export         soft80_kscreen

        .include        "soft80.inc"

soft80_kscreen:
        ldy     #screenrows
        ldx     #charsperline
        rts
