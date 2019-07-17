;
; jede jede@oric.org 2017-02-25
;
    .export    _gotoy

    .import    update_adscr

    .include   "telestrat.inc"

.proc _gotoy
    sta     SCRY
    jsr     update_adscr
    rts
.endproc
