;
; lruntime.s
;
; Ullrich von Bassewitz, 22.06.1998
;

; Runtime support for longs.

	.import	    	popax, pusheax
	.importzp   	sp, sreg, tmp1, tmp2, tmp3, ptr1

;
; leax    (sp),y
;
      	.export	  	ldeax0sp, ldeaxysp

ldeax0sp:
      	ldy 	#3
ldeaxysp:
	lda	(sp),y
	sta	sreg+1
	dey
	lda	(sp),y
	sta	sreg
	dey
	lda	(sp),y
	tax
	dey
	lda	(sp),y
 	rts

;
; push a long from (sp),y
;
	.export		pushlysp

pushlysp:
	iny
	iny
	lda	(sp),y
	iny
	sta	sreg
	lda	(sp),y
	sta	sreg+1
	dey
	dey
	lda	(sp),y
	dey
	tax
	lda 	(sp),y
	jmp	pusheax

;
; eax --> ((sp)); pop
;
	.export	    	steaxspp

steaxspp:
	pha
	txa
	pha
	jsr	popax		; get address
	sta	ptr1
	stx	ptr1+1
	ldy	#3
	lda 	sreg+1
	sta	(ptr1),y
	dey
	lda	sreg
	sta	(ptr1),y
	dey
	pla
	tax
	sta	(ptr1),y
	dey
	pla
	sta	(ptr1),y
	rts

;
; eax --> ((sp)),y
;
	.export	    	steaxspidx

steaxspidx:
   	sta	tmp1
   	stx	tmp2
   	sty	tmp3
   	jsr	popax	   	; get the pointer
   	sta	ptr1
   	stx	ptr1+1
   	ldy	tmp3
   	lda	tmp1
   	sta	(ptr1),y
	iny
	lda	tmp2
	sta	(ptr1),y
	iny
	tax
	lda	sreg
	sta 	(ptr1),y
	iny
	lda	sreg+1
	sta	(ptr1),y
	lda	tmp1
	rts

