;
; jede jede@oric.org 2017-02-25
;
    .export    _wherex

    .importzp  spc

    .include   "telestrat.inc"

.proc _wherex
    ldx    #$00
    lda    SCRX
    rts
.endproc
