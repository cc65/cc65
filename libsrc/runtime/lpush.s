;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: long push
;

;
; push eax on stack
;
	.export		push0ax, pusheax
	.import		decsp4
	.importzp	sp, sreg

push0ax:
	ldy	#0
	sty	sreg
	sty	sreg+1
pusheax:
	jsr	decsp4
	pha
	ldy	#0
	sta	(sp),y
	iny
	txa
	sta	(sp),y
	iny
	lda	sreg
	sta	(sp),y
	iny
	lda	sreg+1
	sta	(sp),y
	pla
	rts

