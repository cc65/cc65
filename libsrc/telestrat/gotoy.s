;
; jede jede@oric.org 2017-02-25
;
    .export    _gotoy

    .importzp  sp

    .include   "telestrat.inc"

.proc _gotoy
   sta     SCRY
   rts
.endproc
