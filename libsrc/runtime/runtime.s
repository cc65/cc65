;
; Runtime code for cc65.
;


	.import		popax
 	.importzp	tmp1, tmp2, tmp3, ptr4


;
; Various kinds of store operators
;
; store AX at SP@@(Y)

	.export		staxspidx, staspidx, staspic
staxspidx:
	jsr	staspic	   	; use common part
	pha
	iny
	lda	tmp2
	sta	(ptr4),y
	tax
	pla
	rts
staspidx:
	jsr	staspic	   	; use common part
	ldx	tmp2
	rts

staspic:
   	sta	tmp1
   	stx	tmp2
   	sty	tmp3
   	jsr	popax	   	; get the pointer
   	sta	ptr4
   	stx	ptr4+1
   	ldy	tmp3
   	lda	tmp1
   	sta	(ptr4),y
   	rts

