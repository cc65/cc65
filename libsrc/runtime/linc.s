;
; Ullrich von Bassewitz, 29.12.1999
;
; CC65 runtime: Increment eax by value in Y
;

       	.export	       	inceaxy
	.importzp	ptr4, sreg

inceaxy:
       	sty    	ptr4
       	clc
       	adc	ptr4
       	bcc	inceax9
       	inx
       	bne	inceax9
       	inc	sreg
       	bne	inceax9
       	inc	sreg+1
inceax9:
       	rts

