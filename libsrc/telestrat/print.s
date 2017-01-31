; jede jede@oric.org 2017-01-22

; void print (char * str);
    .export         _print
    .import         popax
    .importzp       tmp1
    .include        "telestrat.inc"

.proc   _print
    stx tmp1
    ldy tmp1
    BRK_TELEMON XWSTR0
    rts
.endproc


