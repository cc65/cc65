;
; Ullrich von Bassewitz, 17.08.1998
;
; CC65 runtime: division for signed long ints
;

; When negating values, we will ignore the possibility here, that one of the
; values if $80000000, in which case the negate will fail.

       	.export		tosdiveax
	.import		poplsargs, udiv32, adjlsres
	.importzp	ptr1

tosdiveax:
       	jsr	poplsargs  	; Get arguments from stack, adjust sign
       	jsr    	udiv32 		; Do the division
   	lda	ptr1		; Result is in (ptr1:sreg)
   	ldx	ptr1+1
	jmp	adjlsres    	; Adjust the sign of the result if needed

