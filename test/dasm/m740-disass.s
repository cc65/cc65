
    .setcpu "M740"

    .repeat 256, cnt

    .byte 0 + cnt, $12, $02, $ea

    .endrepeat
