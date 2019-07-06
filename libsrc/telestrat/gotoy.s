;
; jede jede@oric.org 2017-02-25
;
    .export    _gotoy

    .import    _update_adscr

    .include   "telestrat.inc"

.proc _gotoy
   sta     SCRY
   jsr     _update_adscr
   rts
.endproc
