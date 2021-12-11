; jede jede@oric.org 2017-01-22

    .export         _close

    .import         addysp,popax

    .include        "zeropage.inc"
    .include        "telestrat.inc"
    .include        "errno.inc"
    .include        "fcntl.inc"

; int open (const char* name, int flags, ...);    /* May take a mode argument */
.proc _close
    BRK_TELEMON XCLOSE  ; launch primitive ROM
    rts
.endproc
