;
; jede jede@oric.org 2017-02-25
;
    .export    _gotox

    .include   "telestrat.inc"

.proc _gotox
   sta    SCRX
   rts
.endproc
