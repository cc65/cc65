;
; Ullrich von Bassewitz, 07.08.1998
;
; CC65 runtime: division for unsigned ints
;

       	.export		tosudiva0, tosudivax, udiv16
	.import		popsreg
	.importzp	sreg, ptr1, ptr4

tosudiva0:
       	ldx	#0
tosudivax:
       	sta	ptr4
       	stx	ptr4+1 	       	; Save right operand
       	jsr	popsreg		; Get left operand

; Do the division

 	jsr	udiv16

; Result is in sreg, remainder in ptr1

 	lda	sreg
 	ldx	sreg+1
 	rts

; Do (sreg/ptr4) -> sreg (see mult-div.s from "The Fridge").
; This is also the entry point for the signed division

udiv16:	lda	#0
 	sta	ptr1+1
 	ldy	#16
L0:	asl	sreg
 	rol	sreg+1
 	rol	a
 	rol	ptr1+1
 	pha
 	cmp	ptr4
 	lda	ptr1+1
 	sbc	ptr4+1
	bcc	L1
	sta	ptr1+1
	pla
	sbc	ptr4
	pha
	inc	sreg
L1:	pla
	dey
	bne	L0
	sta	ptr1
	rts


