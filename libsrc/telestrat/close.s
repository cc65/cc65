; jede jede@oric.org 2017-01-22

    .export         _close

    .include        "telestrat.inc"

.proc _close
    BRK_TELEMON XCLOSE  ; Launch primitive ROM
    rts
.endproc
