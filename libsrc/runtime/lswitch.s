;
; Ullrich von Bassewitz, 17.08.1998
;
; CC65 runtime: switch statement with long selector
;

; Subroutine to handle a switch statement with an int selector. The table
; is located at the return address from the function. It contains the negative
; of the case label count as first word, followed by three words for each case
; label, the first two being the value, and the second one the label to jump
; to in case of a match. The default case is located at the end of the table.

	.export		lswitch
	.importzp	sreg, ptr1, ptr2, ptr3

lswitch:
	sta	ptr1
       	stx    	ptr1+1		; Save AX
	clc
	pla
	adc	#1
	sta	ptr2
	pla
	adc	#0
	sta	ptr2+1		; Get pointer to table

	ldy	#0
	lda	(ptr2),y
	sta	ptr3
	iny
	lda	(ptr2),y
	sta	ptr3+1	   	; Remember the count of labels

	ldy	#0
	clc	  	   	; Skip the label count
	lda	ptr2
	adc	#2
	sta	ptr2
	bcc	L2
	inc	ptr2+1
	bne	L2	   	; Branch always

; Search for the label

L0: 	lda	(ptr2),y
   	cmp	ptr1
       	bne    	L1
   	iny
   	lda	(ptr2),y
   	cmp	ptr1+1
   	bne	L1
   	iny
   	lda	(ptr2),y
   	cmp	sreg
   	bne	L1
   	iny
   	lda	(ptr2),y
   	cmp	sreg+1
   	beq	L3
L1:	clc
  	lda	ptr2
  	adc	#6	   	; Skip table entry
  	sta	ptr2
  	bcc	L2
  	inc	ptr2+1

; Check if there are any labels left

L2:	inc	ptr3
   	bne	L0
   	inc	ptr3+1
   	bne	L0

; Out of labels

	jmp	(ptr2)

; Label found

L3:   	ldy	#4	   	; Jump label offset
  	lda	(ptr2),y
  	sta	ptr3
  	iny
  	lda	(ptr2),y
  	sta	ptr3+1
  	jmp	(ptr3)

