;
; Ullrich von Bassewitz, 20.09.1998
;
; CC65 runtime: left shift support for longs
;


    	.export	       	tosasleax, tosshleax
    	.import	       	addysp1
	.importzp      	sp, sreg, ptr1, ptr2


tosshleax:
tosasleax:

; Get the lhs from stack into ptr1/ptr2

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
       	jsr   	addysp1

; Check for shift overflow or zero shift

       	tay    	       	       	; Low byte of shift count into y
       	txa	    	       	; Get byte 2
	ora	sreg
       	ora    	sreg+1	       	; Check high 24 bit
	bne	@L6            	; Shift count too large
	cpy	#32
	bcs    	@L6

   	cpy  	#0  	       	; Shift count zero?
   	beq  	@L5

; We must shift. Shift by multiples of eight if possible

   	tya
@L1:  	cmp  	#8
   	bcc  	@L3
   	sbc  	#8
       	ldx  	ptr2
   	stx  	ptr2+1
   	ldx  	ptr1+1
   	stx  	ptr2
   	ldx  	ptr1
	stx	ptr1+1
	ldx	#0
	stx	ptr1
       	beq    	@L1

; Shift count is now less than eight. Do a real shift.

@L3:	tay  	     	       	; Shift count to Y
	lda    	ptr1	       	; Get one byte into A for speed
	cpy	#0
       	beq    	@L4a 	       	; Jump if done
@L4:	asl	a
   	rol    	ptr1+1
   	rol    	ptr2
   	rol  	ptr2+1
   	dey
   	bne  	@L4

; Put the result in place

@L4a:  	ldx	ptr2
	stx    	sreg
	ldx	ptr2+1
	stx	sreg+1
	ldx	ptr1+1
@L5:	rts

; Jump here if shift count overflow

@L6:	lda    	#0
 	sta	sreg+1
 	sta	sreg
 	tax
 	rts




