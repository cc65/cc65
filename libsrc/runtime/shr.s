;
; Ullrich von Bassewitz, 05.08.1998
;
; CC65 runtime: right shift support for ints
;


; --------------------------------------------------------------------
; signed shift

  	.export		tosasra0, tosasrax
	.import		popsreg, return0
	.importzp	sreg

tosasra0:
  	ldx	#0
tosasrax:
  	jsr	popsreg		; get TOS into sreg
  	cpx	#0
  	bne	TooLarge
  	cmp	#16
  	bcs	TooLarge

       	cmp 	#8		; Shift count greater 8?
	beq	L4		; Jump if exactly 8
       	bcc    	L2		; Jump if no

; Shift count is greater 8. Do the first 8 bits the fast way

   	ldy    	#0
   	ldx	sreg+1
   	stx	sreg
   	bpl	L1
   	dey	  		; Create correct sign bits
L1:	sty	sreg+1
   	sec
   	sbc	#8

; Shift count less than 8 if we come here

L2:	tay			; Shift count --> Y
	beq    	Zero		; Done if shift count zero

  	lda	sreg		; get low byte for faster shift
  	ldx	sreg+1		; get high byte

; Do the actual shift

L3:	cpx	#$80		; get bit 7 into carry
  	ror	sreg+1
  	ror	a
	dey
       	bne	L3

; Done with shift

  	ldx	sreg+1
  	rts

; Shift count == 8

L4:	lda	sreg+1		; X is zero
	bpl	*+3
	dex			; X == 0xFF
	rts

; Shift count was zero

Zero:	lda	sreg
	ldx	sreg+1
	rts

; Shift count too large, result is zero

TooLarge:
	jmp	return0


; --------------------------------------------------------------------
; unsigned shift

  	.export		tosshra0, tosshrax

tosshra0:
      	ldx	#0
tosshrax:
  	jsr	popsreg		; get TOS into sreg
  	cpx	#0
  	bne	TooLarge
  	cmp	#16
  	bcs	TooLarge

       	cmp 	#8		; Shift count greater 8?
	beq	L8		; Jump if exactly 8
       	bcc    	L6		; Jump if no

; Shift count is greater 8. Do the first 8 bits the fast way

   	sbc	#8		; Carry already set
   	ldy	sreg+1
   	sty	sreg
       	stx	sreg+1		; High byte = 0

; Shift count less than 8 if we come here

L6:	tay	  		; Shift count --> Y
  	beq    	Zero		; Done if shift count zero

  	lda	sreg		; get low byte for faster shift

; Do the actual shift

L7:    	lsr	sreg+1
  	ror	a
  	dey
       	bne	L7

; Done with shift

  	ldx	sreg+1
  	rts

; Shift count == 8

L8:	lda	sreg+1		; X is zero
	rts

