;
; Ullrich von Bassewitz, 16.03.2002
;
; CC65 runtime: Multiply the primary register
;

   	.export	    	mulax7
	.importzp      	ptr1, tmp1

.proc	mulax7

	sta	ptr1
	stx	ptr1+1
	stx   	tmp1
	asl   	a
	rol   	tmp1	    		; * 2
	asl	a
	rol	tmp1			; * 4
	asl	a
	rol	tmp1			; * 8
	sec
	sbc	ptr1
	pha
	lda	tmp1
	sbc	ptr1+1			; * (8 - 1)
	tax
	pla
	rts

.endproc


