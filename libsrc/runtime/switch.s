;
; Ullrich von Bassewitz, 17.08.1998
;
; CC65 runtime: switch statement with int selector
;

; Subroutine to handle a switch statement with an int selector. The table
; is located at the return address from the function. It contains the negative
; of the case label count as first word, followed by two words for each case
; label, the first one being the value, and the second one the label to jump
; to in case of a match. The default case is located at the end of the table.

	.export		switch
	.importzp	ptr1, ptr2, ptr3

switch:	sta	ptr1
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
	sta	ptr3+1		; Remember the count of labels

	ldy	#0
	clc	  		; Skip the label count
	lda	ptr2
	adc	#2
	sta	ptr2
	bcc	L2
	inc	ptr2+1
	bne	L2		; Branch always

; Search for the label

L0:	lda	(ptr2),y
   	iny
   	cmp	ptr1
       	beq	L4
L1:  	iny
   	iny
   	iny	  		; Overflow only here
   	bne	L2
   	inc	ptr2+1		; Bump high byte

; Check if there are any labels left

L2:	inc	ptr3
   	bne	L0
   	inc	ptr3+1
   	bne	L0

; Out of labels

  	tya
  	clc
  	adc	ptr2
  	sta	ptr2
  	bcc	L3
  	inc	ptr2+1
L3:	jmp	(ptr2)


; Check high byte

L4:   	lda	(ptr2),y
	cmp	ptr1+1
       	bne    	L1

; Label found

   	iny
	lda	(ptr2),y
	sta	ptr3
	iny
	lda	(ptr2),y
	sta	ptr3+1
	jmp	(ptr3)


