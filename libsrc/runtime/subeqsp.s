;
; Ullrich von Bassewitz, 08.10.1998
;
; CC65 runtime: -= operator for ints on the stack
;

       	.export	       	subeq0sp, subeqysp
   	.importzp	sp, tmp1, tmp2

subeq0sp:
   	ldy	#0
subeqysp:
	sec
	sta	tmp1
	stx	tmp2
	lda    	(sp),y
	sbc	tmp1
	sta	(sp),y
	pha
	iny
       	lda	(sp),y
	sbc	tmp2
	sta	(sp),y
	tax
	pla
	rts

