;
; Ullrich von Bassewitz, 07.08.1998
;
; CC65 runtime: helper stuff for mod/div/mul with signed ints
;

; When negating values, we will ignore the possibility here, that one of the
; values if $8000, in which case the negate will fail.

       	.export		popsargs, adjsres
	.import		negax, popax
	.importzp	sreg, tmp1, tmp2, ptr4

popsargs:
      	stx	tmp1	       	; Remember sign
      	cpx	#0
      	bpl	L1
      	jsr	negax	       	; Negate accumulator
L1:   	sta	ptr4
      	stx	ptr4+1 	       	; Save right operand

      	jsr	popax
      	stx	tmp2	       	; Remember sign
      	cpx	#0
      	bpl	L2
      	jsr	negax
L2:   	sta	sreg
      	stx	sreg+1

; Calculate the sign of the result, that is sign(op1) * sign(op2)

      	lda	tmp1
      	eor	tmp2
      	sta	tmp2  	       	; Save it across call
L3:	rts

; Adjust the result of a mod/div/mul operation

adjsres:

; Check if we must adjust the sign

      	ldy	tmp2
      	bpl	L3
      	jmp    	negax 	       	; Adjust sign



