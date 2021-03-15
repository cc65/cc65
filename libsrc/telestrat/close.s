; jede jede@oric.org 2017-01-22

    .export         _close

    .include        "zeropage.inc"
    .include        "telestrat.inc"

; int open (const char* name, int flags, ...);    /* May take a mode argument */
.proc _close
    BRK_TELEMON XCLOSE  ; Launch primitive ROM
    rts
.endproc
