;
; Ullrich von Bassewitz, 29.12.1999
;
; CC65 runtime: Load eax from immidiate value following the call
;

	.export		ldeax
	.importzp	sreg, ptr4


ldeax:	pla			; Low byte of return address
	sta	ptr4
	pla			; high byte of return address
	sta	ptr4+1
	ldy	#4		; high byte of value
	lda	(ptr4),y
	sta	sreg+1
	dey
	lda	(ptr4),y
	sta	sreg
	dey
	lda	(ptr4),y
	tax
	dey
	lda	(ptr4),y
	tay			; Save low byte
	clc
	lda	#4
	adc	ptr4
	sta	ptr4
	lda	ptr4+1
	adc	#$00
	pha			; High byte of new return address
	lda	ptr4
	pha			; Low byte of new return address
	tya			; Low byte of fetched value
	rts

