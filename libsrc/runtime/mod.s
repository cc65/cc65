;
; Ullrich von Bassewitz, 07.08.1998
;
; CC65 runtime: modulo operation for signed ints
;

; When negating values, we will ignore the possibility here, that one of the
; values if $8000, in which case the negate will fail.

       	.export		tosmoda0, tosmodax
	.import		popsargs, udiv16, adjsres
	.importzp	ptr1

tosmoda0:
      	ldx	#0
tosmodax:
	jsr	popsargs	; Get arguments from stack, adjust sign
      	jsr	udiv16		; Do the division
      	lda	ptr1		; Result is in sreg, remainder in ptr1
      	ldx	ptr1+1
	jmp	adjsres		; Adjust the sign of the result if needed


