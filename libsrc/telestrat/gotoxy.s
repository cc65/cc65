;
; jede jede@oric.org 2017-02-25
;
    .export    _gotoxy
	
    .import    popa
	
    .importzp  sp

    .include   "telestrat.inc"

.proc _gotoxy
   ; This function move only cursor for display, it does not move the prompt position
   ; in telemon, there is position for prompt, and another for the cursor
   sta    SCRY
   jsr    popa
   sta    SCRX
   rts
.endproc	
