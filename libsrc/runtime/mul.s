;
; Ullrich von Bassewitz, 07.08.1998
;
; CC65 runtime: multiplication for ints
;

       	.export		tosumula0, tosumulax, tosmula0, tosmulax
	.import		popsreg
	.importzp	sreg, tmp1, ptr4

tosmula0:
tosumula0:
	ldx	#0
tosmulax:
tosumulax:
mul16:	sta	ptr4
      	stx	ptr4+1 	       	; Save right operand
      	jsr	popsreg	       	; Get left operand

; Do ptr4*sreg --> AX (see mult-div.s from "The Fridge").

	lda	#0
   	sta	tmp1
	ldx	sreg+1	       	; Get into register for speed
   	ldy    	#16 	       	; Number of bits
L0:	lsr	tmp1
   	ror	a
   	ror	ptr4+1
   	ror	ptr4
   	bcc	L1
   	clc
   	adc	sreg
   	pha
       	txa	    	       	; hi byte of left op
   	adc	tmp1
   	sta	tmp1
   	pla
L1:	dey
       	bpl    	L0
	lda	ptr4	       	; Load the result
	ldx	ptr4+1
	rts			; Done

