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
	sta	ptr1
	stx	ptr1+1
.ifpc02
      	lda	(sp),y		; Get lo byte
      	ldy	#1		; Hi index
.else
      	ldy	#0
      	lda	(sp),y		; Lo byte
      	iny			; Hi index
.endif
	sec			   
	sbc	ptr1
	sta	ptr1		; save lo byte
	lda	(sp),y
	sbc	ptr1+1
	tax
	lda	ptr1
	jmp	addysp1		; drop TOS, set condition codes

