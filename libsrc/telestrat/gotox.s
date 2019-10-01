;
; jede jede@oric.org 2017-02-25
;
    .export    _gotox
    .import    OLD_CHARCOLOR, OLD_BGCOLOR

    .include   "telestrat.inc"

.proc _gotox
   sta    SCRX

   lda     #$FF
   sta     OLD_CHARCOLOR         
   sta     OLD_BGCOLOR          
   rts
.endproc
