;
; Ullrich von Bassewitz, 08.10.1998
;
; CC65 runtime: -= operator for longs on the stack
;			    

       	.export	       	lsubeq0sp, lsubeqysp
	.importzp   	sp, sreg, tmp1, tmp2

lsubeq0sp:
      	ldy	#0
lsubeqysp:
    	sec
	sta	tmp1
	stx	tmp2
       	lda	(sp),y
	sbc	tmp1
	sta	(sp),y
	pha
	iny
       	lda	(sp),y
	sbc	tmp2
	sta	(sp),y
	tax
	iny
       	lda	(sp),y
	sbc	sreg
	sta	(sp),y
	sta	sreg
	iny
       	lda	(sp),y
	sbc	sreg+1
	sta	(sp),y
	sta	sreg+1
	pla
	rts

