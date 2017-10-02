;
; jede jede@oric.org 2017-10-01
;
    .export    _cgetc
   
    .include   "telestrat.inc"

.proc _cgetc
loop:   
        BRK_TELEMON XRD0
        bcs     loop
        rts
.endproc	
