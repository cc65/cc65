;
; Ullrich von Bassewitz, 07.08.1998
;
; CC65 runtime: modulo operation for long signed ints
;

; When negating values, we will ignore the possibility here, that one of the
; values if $8000, in which case the negate will fail.

       	.export		tosmodeax
	.import		poplsargs, udiv32, adjlsres
	.importzp	sreg, ptr1, ptr2, tmp3, tmp4

tosmodeax:
       	jsr    	poplsargs	; Get arguments from stack, adjust sign
      	jsr	udiv32		; Do the division
      	lda	ptr1   	       	; Remainder is in (ptr2:tmp3:tmp4)
	lda	ptr2
	ldx	ptr2+1
	ldy    	tmp3
	sty	sreg
	ldy	tmp4
	sty	sreg+1
	jmp	adjlsres	; Adjust the sign of the result if needed


