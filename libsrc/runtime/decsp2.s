;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Decrement the stackpointer by 2
;

       	.export	  	decsp2
	.importzp	sp

.proc	decsp2

	ldy	sp
       	beq	@L1
       	dey
       	beq	@L2
       	dey
       	sty	sp
       	rts

@L1:	dey
@L2:    dey
       	sty	sp
       	dec	sp+1
	rts

.endproc
	



	
