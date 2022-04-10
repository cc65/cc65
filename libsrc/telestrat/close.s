; jede jede@oric.org 2017-01-22

        .export         _close

        .import         addysp,popax

        .include        "zeropage.inc"
        .include        "telestrat.inc"
        .include        "errno.inc"
        .include        "fcntl.inc"

; int __fastcall__ close (int fd);
.proc _close
        jsr     popax       ; Get fd
        BRK_TELEMON XCLOSE  ; Launch primitive ROM
        rts
.endproc
