;
; Ullrich von Bassewitz, 27.09.1998
;
; CC65 runtime: modulo operation for long unsigned ints
;

       	.export		tosumodeax
       	.import		getlop, udiv32
       	.importzp	sreg, tmp3, tmp4, ptr2

tosumodeax:
	jsr	getlop		; Get the paramameters
 	jsr	udiv32		; Do the division
 	lda	tmp3		; Remainder is in ptr2:tmp3:tmp4
	sta	sreg
	lda	tmp4
	sta	sreg
	lda	ptr2
	ldx	ptr2+1
	rts

