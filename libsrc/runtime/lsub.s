;
; Ullrich von Bassewitz, 05.08.1998
;
; CC65 runtime: long sub
;

;
; EAX = TOS - EAX
;
	.export		tossubeax
	.import		addysp1
	.importzp	sp, sreg, tmp1, tmp2

tossubeax:
      	ldy	#0
       	sec
	sta	tmp1
	lda	(sp),y
	sbc	tmp1	      	; byte 0
	sta	tmp2	      	; use as temp storage
	iny
	stx	tmp1
	lda	(sp),y
	sbc	tmp1	      	; byte 1
	tax
	iny
	lda	(sp),y
	sbc	sreg	      	; byte 2
	sta	sreg
	iny
	lda	(sp),y
	sbc	sreg+1	      	; byte 3
	sta	sreg+1
	lda	tmp2	      	; load byte 0
       	jmp    	addysp1	      	; drop TOS

