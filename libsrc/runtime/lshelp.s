;
; Ullrich von Bassewitz, 13.08.1998
;
; CC65 runtime: helper stuff for mod/div/mul with long signed ints
;

; When negating values, we will ignore the possibility here, that one of the
; values if $80000000, in which case the negate will fail.

       	.export		poplsargs, adjlsres
	.import		getlop, negeax
	.importzp	sreg, tmp1, ptr1, ptr3, ptr4

poplsargs:
	jsr	getlop	   	; Get the operands

; Calculate the sign of the result, that is sign(op1) * sign(op2) and
; remember it.

      	lda   	sreg+1
      	eor   	ptr4+1
      	sta   	tmp1  	   	; Save it across call

; Make both operands positive

	lda   	sreg+1		; Is the operand negative?
	bpl   	L1		; Jump if not

	clc			; Make it positive
	lda	ptr1
	eor	#$FF
	adc	#$01
	sta	ptr1
	lda	ptr1+1
	eor	#$FF
	adc	#$00
	sta	ptr1+1
	lda	sreg
	eor	#$FF
	adc	#$00
	sta	sreg
	lda	sreg+1
	eor	#$FF
	adc	#$00
	sta	sreg+1

L1:	lda	ptr4+1		; Is the operand nagative?
	bpl	L2		; Jump if not

	clc			; Make it positive
	lda	ptr3
	eor	#$FF
	adc	#$01
	sta	ptr3
	lda	ptr3+1
	eor	#$FF
	adc    	#$00
	sta	ptr3+1
	lda	ptr4
	eor	#$FF
	adc	#$00
	sta	ptr4
	lda	ptr4+1
	eor	#$FF
	adc	#$00
	sta	ptr4+1

L2:	rts

; Adjust the result of a mod/div/mul operation

adjlsres:
      	ldy	tmp1		; Check if we must adjust the sign
      	bpl	L2
      	jmp    	negeax 		; Netage value

