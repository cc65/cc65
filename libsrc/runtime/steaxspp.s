;
; Ullrich von Bassewitz, 22.06.1998
;
; CC65 runtime: Store eax at the address on top of stack.
;


	.export	    	steaxspp
	.import		popax
	.importzp	sreg, ptr1

.proc	steaxspp

	pha
	txa
	pha
	jsr	popax		; get address
	sta	ptr1
	stx	ptr1+1
	ldy	#3
	lda 	sreg+1
	sta	(ptr1),y
	dey
	lda	sreg
	sta	(ptr1),y
	dey
	pla
	tax
	sta	(ptr1),y
	dey
	pla
	sta	(ptr1),y
	rts

.endproc


