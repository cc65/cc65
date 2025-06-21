
    .setcpu "6502"

    .repeat 256, cnt

    .byte 0 + cnt, $02, $ea, $00

    .endrepeat
