;
; Ullrich von Bassewitz, 05.08.1998
;
; CC65 runtime: sub ints
;

       	.export		tossuba0, tossubax
	.import		addysp1		
	.importzp	sp, ptr1

;
; AX = TOS - AX
;

tossuba0:
	ldx	#0
tossubax:
	ldy	#0
	sta	ptr1
	stx	ptr1+1
	lda	(sp),y		; lo byte
	sec
	sbc	ptr1
	sta	ptr1		; save lo byte
	iny
	lda	(sp),y
	sbc	ptr1+1
	tax
	lda	ptr1
	jmp	addysp1		; drop TOS, set condition codes

