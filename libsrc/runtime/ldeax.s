;
; Ullrich von Bassewitz, 29.12.1999
;
; CC65 runtime: Load eax from immidiate value following the call
;

	.export		ldeax
	.importzp	sreg, ptr4

	.macpack	generic

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

; Calculate the return address (remember: RTS address is one low) and 
; jump to it

	lda	ptr4
	add	#5
	sta	ptr4
	bcc	@L9
	inc	ptr4+1
@L9:	jmp	(ptr4)
