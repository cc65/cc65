    .export    _gotoxy
	
    .import    popa
	
    .importzp  sp,tmp2,tmp3,tmp1

    .include   "telestrat.inc"


.proc _gotoxy
   sta SCRY
   jsr popa
   sta SCRX
   rts
.endproc	
