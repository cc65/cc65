;
; Ullrich von Bassewitz, 09.12.1998
;
; void __fastcall__ _swap (void* p, void* q, size_t size);
;

	.export	       	__swap
	.import		popax
	.importzp	ptr1, ptr2, ptr3


__swap:	sta	ptr3		; Save size
	stx	ptr3+1

	jsr	popax		; Get q
	sta	ptr2
	stx	ptr2+1

	jsr	popax		; Get p
	sta	ptr1
	stx	ptr1+1

; Prepare for swap

	ldy	#$00

; Swap 256 byte blocks

	ldx	ptr3+1
	beq	@L2

@L1:	lda	(ptr1),y
    	tax
    	lda	(ptr2),y
    	sta	(ptr1),y
    	txa
    	sta	(ptr2),y
    	iny
    	bne	@L1
	dec	ptr3+1
	bne	@L1

; Swap remaining bytes (Y is zero)

@L2:	ldx	ptr3
	beq	@L9

@L3:   	lda	(ptr1),y
    	tax
    	lda	(ptr2),y
    	sta	(ptr1),y
    	txa
    	sta	(ptr2),y
	iny
    	dec	ptr3
    	bne	@L3

; Done

@L9:	rts


