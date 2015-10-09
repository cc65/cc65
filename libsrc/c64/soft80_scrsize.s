
        .export         soft80_screensize

        .include        "soft80.inc"

soft80_screensize:
        ldy     #screenrows
        ldx     #charsperline
        rts

;-------------------------------------------------------------------------------
; force the init constructor to be imported

        .import soft80_init
conio_init      = soft80_init
