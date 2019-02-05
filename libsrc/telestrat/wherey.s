;
; jede jede@oric.org 2017-02-25
;
    .export    _wherey

    .include   "telestrat.inc"

.proc _wherey
    ldx    #$00
    lda    SCRY
    rts
.endproc
