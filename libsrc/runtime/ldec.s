;
; Ullrich von Bassewitz, 29.12.1999
;
; CC65 runtime: Decrement eax by value in Y
;

       	.export	       	deceaxy
	.importzp	ptr4, sreg

deceaxy:
	sty	ptr4
	sec
	sbc	ptr4
	sta	ptr4
	txa
	sbc	#0
	tax
	lda	sreg
	sbc	#0
	sta	sreg
	lda	sreg+1
	sbc	#0
	sta	sreg+1
	lda	ptr4
	rts

