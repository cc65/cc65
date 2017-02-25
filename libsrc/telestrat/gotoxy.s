    .export    _gotoxy
	
    .import    popa
	
    .importzp  sp,tmp2,tmp3,tmp1

    .include   "telestrat.inc"


.proc _gotoxy
   ; This function move only cursor for display, it does not move the prompt position
   ; in telemon, there is position for prompt, and another for the cursor
   sta SCRY
   jsr popa
   sta SCRX
   rts
.endproc	
