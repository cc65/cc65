;
; Ullrich von Bassewitz, 05.08.1998
;
; CC65 runtime: boolean negation
;

       	.export		bnega, bnegax
	.import		return0, return1

bnegax:	cpx	#0
  	bne	L0
bnega:	cmp	#0
  	bne	L0
L1:	ldx	#0
   	lda	#1
   	rts

L0:	ldx	#0
	txa
	rts

