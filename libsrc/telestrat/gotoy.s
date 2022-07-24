;
; jede jede@oric.org 2017-02-25
;
    .export    _gotoy

    .import    update_adscr

    .include   "telestrat.inc"

.proc _gotoy
    sta     SCRY
    jmp     update_adscr
.endproc
