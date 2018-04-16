; 2018-04-13, Jede (jede@oric.org)
;

; void cputc (char c);
;

        .export         _cputc

        .include        "telestrat.inc"

.proc _cputc
    BRK_TELEMON  XWR0       ; macro send char to screen (channel 0 in telemon terms)
    rts
.endproc

