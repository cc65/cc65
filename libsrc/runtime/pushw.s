;
; Ullrich von Bassewitz, 31.08.1998
;
; CC65 runtime: Push word from stack
;

	.export		pushw, pushwidx
	.import		pushax
	.importzp	ptr1

pushwidx:
	sty	ptr1
	clc
	adc	ptr1
	bcc	pushw
	inx
pushw:	sta	ptr1
	stx	ptr1+1
	ldy	#1
	lda	(ptr1),y
	tax
	dey
	lda	(ptr1),y
	jmp	pushax

