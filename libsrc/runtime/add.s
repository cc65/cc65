;
; Ullrich von Bassewitz, 05.08.1998
;
; CC65 runtime: add ints
;

; Make this as fast as possible, even if it needs more space since it's
; called a lot!

       	.export		tosadda0, tosaddax
	.importzp	sp, tmp1

tosadda0:
	ldx	#0
tosaddax:
	ldy	#0
	clc
	adc	(sp),y		; lo byte
	sta	tmp1		; save it
	txa
	iny
	adc	(sp),y		; hi byte
	tax
	clc
	lda	sp
	adc	#2
	sta	sp
	bcc	L1
	inc	sp+1
L1:	txa	    		; Test high byte
	bmi	L2
	bne	L3
	lda	tmp1		; Get low byte
	rts

; Value is negative

L2:	lda	tmp1		; Get low byte
	ldy	#$FF		; Force negative
	rts

; Value is positive != 0

L3:	lda	tmp1		; Get low byte
	ldy	#1
	rts

