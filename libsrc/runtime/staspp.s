;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Store a indirect into address at top of stack
;

       	.export	       	staspp
	.import		incsp2
	.importzp	sp, ptr1

.proc	staspp

	ldy   	#1
	pha
	lda   	(sp),y
	sta   	ptr1+1
	dey
	lda   	(sp),y
	sta	ptr1
	pla
	sta	(ptr1),y
       	jmp	incsp2	       	; Drop address

.endproc


