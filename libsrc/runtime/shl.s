;
; Ullrich von Bassewitz, 05.08.1998
;
; CC65 runtime: left shift support for ints
;

	.export		tosasla0, tosaslax, tosshla0, tosshlax
	.import		popsreg, return0
	.importzp	sreg

tosshla0:
tosasla0:
	ldx	#0
tosshlax:
tosaslax:
  	jsr	popsreg		; get TOS into sreg
  	cpx	#0
  	bne	TooLarge
  	cmp	#16
  	bcs	TooLarge

       	cmp 	#8		; Shift count greater 8?
	beq	L3		; Jump if exactly 8
       	bcc    	L1		; Jump if no

; Shift count is greater 8. Do the first 8 bits the fast way

   	ldy	sreg
   	sty	sreg+1
       	stx	sreg		; Low byte = 0
   	sec
   	sbc	#8

; Shift count less than 8 if we come here

L1:	tay	  		; Shift count --> Y
  	beq    	Zero		; Done if shift count zero

  	lda	sreg		; get low byte for faster shift

; Do the actual shift

L2:    	asl	a
	rol	sreg+1
  	dey
       	bne	L2

; Done with shift

  	ldx	sreg+1
  	rts

; Shift count == 8

L3:	txa			; X == 0, now A == 0
	ldx	sreg
	rts

; Shift count was zero

Zero:	lda	sreg
	ldx	sreg+1
	rts

; Shift count too large, result is zero

TooLarge:
	jmp	return0

