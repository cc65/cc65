;
; Ullrich von Bassewitz, 16.03.2002
;
; CC65 runtime: Multiply the primary register
;

   	.export	      	mulax6
	.importzp     	ptr1

.proc	mulax6

	sta     ptr1
	stx	ptr1+1
	asl	a
	rol	ptr1+1
	clc
	adc	ptr1
	pha
	txa
	adc	ptr1+1
	sta	ptr1+1
	pla
	asl	a
	rol	ptr1+1
 	ldx	ptr1+1
	rts

.endproc


