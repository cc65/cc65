;	
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Store a/x indirect into address at top of stack
;

       	.export		staxspp
	.import		incsp2
	.importzp	sp, ptr1

.proc	staxspp

	ldy	#0
	pha
	lda	(sp),y
	sta	ptr1
	iny
	lda	(sp),y
	sta	ptr1+1
	txa
	sta     (ptr1),y
	pla
	dey
	sta	(ptr1),y
	jmp	incsp2		; Drop address

.endproc


