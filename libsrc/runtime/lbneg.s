;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: boolean negation for longs
;

 	.export		bnegeax
	.import		return0, return1
	.importzp	sreg

bnegeax:
	cmp	#0
	bne	L1
	cpx	#0
	bne	L1
       	lda 	sreg
	bne	L1
       	lda	sreg+1
	bne	L1
	jmp	return1
L1:	jmp	return0

