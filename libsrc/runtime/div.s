;
; Ullrich von Bassewitz, 07.08.1998
;
; CC65 runtime: division for signed ints
;

; When negating values, we will ignore the possibility here, that one of the
; values if $8000, in which case the negate will fail.

       	.export		tosdiva0, tosdivax
	.import		popsargs, udiv16, adjsres
	.importzp	sreg

tosdiva0:
      	ldx	#0
tosdivax:
	jsr	popsargs	; Get arguments from stack, adjust sign
      	jsr	udiv16		; Do the division
   	lda	sreg		; Result is in sreg, remainder in ptr1
   	ldx	sreg+1
	jmp	adjsres	      	; Adjust the sign of the result if needed

