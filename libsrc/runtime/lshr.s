;
; Ullrich von Bassewitz, 20.09.1998
;
; CC65 runtime: right shift support for longs
;


    	.export	  	tosasreax, tosshreax
    	.import	       	addysp1
	.importzp      	sp, sreg, ptr1, ptr2

; --------------------------------------------------------------------
; signed shift

.proc	tosasreax

       	jsr  	getlhs	       	; Get the lhs from the stack

       	jsr   	checkovf       	; Check for overflow
   	bcs    	L6  	       	; Jump if shift count too large

   	cpy  	#0	       	; Shift count zero?
   	beq  	L5

; We must shift. Shift by multiples of eight if possible

   	tya
L1:  	cmp  	#8
   	bcc  	L3
   	sbc  	#8
       	ldx    	ptr1+1
   	stx  	ptr1
   	ldx  	ptr2
   	stx  	ptr1+1
   	ldy  	#0
   	ldx  	ptr2+1
	stx	ptr2
   	bpl	L2
   	dey	  	       	; Get sign
L2:	sty	ptr2+1
   	jmp	L1

; Shift count is now less than eight. Do a real shift.
		
L3:	tay	  	       	; Shift count to Y
	lda    	ptr2+1       	; Get one byte into A for speed
	cpy	#0
       	beq    	L4a	       	; Jump if done
L4: 	cmp	#$80	       	; Get sign bit into C
    	ror	a
    	ror	ptr2
    	ror	ptr1+1
    	ror	ptr1
    	dey
    	bne	L4

; Put the result in place

L4a:	sta	sreg+1
   	lda	ptr2
   	sta	sreg
   	ldx	ptr1+1
   	lda	ptr1
L5:	rts

; Jump here if shift count overflow

L6:	ldx	#0
 	lda	ptr2+1       	; Check sign
       	bpl    	L7
	dex
L7:	stx	sreg+1
 	stx	sreg
 	txa
 	rts

.endproc

; --------------------------------------------------------------------
; unsigned shift

.proc	tosshreax

       	jsr  	getlhs	       	; Get the lhs from the stack

   	jsr  	checkovf       	; Check for overflow
       	bcs    	L6  	       	; Jump if shift count too large

   	cpy  	#0  	       	; Shift count zero?
   	beq  	L5

; We must shift. Shift by multiples of eight if possible

   	tya
L1:  	cmp  	#8
   	bcc  	L3
   	sbc  	#8
       	ldx  	ptr1+1
   	stx  	ptr1
   	ldx  	ptr2
   	stx  	ptr1+1
   	ldx  	ptr2+1
	stx	ptr2
	ldx	#0
	stx	ptr2+1
       	beq    	L1

; Shift count is now less than eight. Do a real shift.

L3:	tay  	    		; Shift count to Y
	lda    	ptr2+1		; Get one byte into A for speed
	cpy	#0
       	beq    	L4a		; Jump if done
L4:	lsr	a
   	ror  	ptr2
   	ror  	ptr1+1
   	ror  	ptr1
   	dey
   	bne  	L4

; Put the result in place

L4a: 	sta	sreg+1
   	lda	ptr2
   	sta	sreg
   	ldx	ptr1+1
   	lda	ptr1
L5:	rts

; Jump here if shift count overflow

L6:	lda    	#0
 	sta	sreg+1
 	sta	sreg
 	tax
 	rts

.endproc

; --------------------------------------------------------------------
; Helpers

.proc  	getlhs	 		; Get the lhs from stack into ptr1/ptr2

 	pha
 	ldy   	#0
 	lda   	(sp),y
 	sta   	ptr1
	iny
	lda   	(sp),y
	sta   	ptr1+1
	iny
	lda   	(sp),y
	sta   	ptr2
	iny
	lda   	(sp),y
	sta   	ptr2+1
	pla
       	jmp   	addysp1

.endproc


.proc	checkovf     		; Check for shift overflow

       	tay    	       		; Low byte of shift count into y
       	txa	     	      	; Get byte 2
	ora	sreg
	ora	sreg+1  	; Check high 24 bit
	bne	TooLarge      	; Shift count too large
	cpy	#32
	bcc    	Ok
TooLarge:
	sec
Ok:	rts

.endproc








