;
; Runtime code for cc65.
;


 	.import		ldai, ldaxi, pushax
 	.importzp	sp, tmp1, tmp2, tmp3, ptr1, ptr4


; Pop a from stack
 	.export		popa

popa:	ldy	#0
 	lda	(sp),y		; Read byte
	inc	sp
	bne	*+4
	inc	sp+1
	rts

;
; pop a from stack and load x with zero
;

	.export		popa0

popa0:	ldy	#0
	lda	(sp),y		; load low byte
	ldx	#0
	beq	incsp2

;
; pop a/x from stack. This function will run directly into incsp2
;

   	.export		popax		; pop stack into AX

popax:	ldy	#1
   	lda	(sp),y		; get hi byte
       	tax	     		; into x
   	dey
   	lda	(sp),y		; get lo byte

;
; routines for inc/dec'ing sp
;

   	.export	 	addysp, addysp1
       	.export	 	incsp1, incsp2, incsp3, incsp4
   	.export	 	incsp5, incsp6, incsp7, incsp8

; do this by hand, cause it gets used a lot

incsp2:	ldy	sp		; 3
       	iny			; 2
       	beq    	@L1		; 2
       	iny			; 2
       	beq	@L2		; 2
       	sty	sp		; 3
       	rts

@L1:   	iny			; 2
@L2:   	sty	sp		; 3
       	inc	sp+1		; 5
       	rts

; Hand optimize this one also...

incsp1:	inc    	sp
 	bne    	*+4
 	inc    	sp+1
 	rts

incsp3:	ldy    	#3
 	bne    	addysp

incsp4:	ldy	#4
 	bne	addysp

incsp5:	ldy	#5
 	bne	addysp

incsp6:	ldy	#6
 	bne	addysp

incsp7:	ldy	#7
 	bne	addysp

incsp8:	ldy	#8
 	bne	addysp

addysp1:
	iny
addysp:	pha		       	; save A
 	clc
 	tya		       	; get the value
 	adc	sp	       	; add lo byte
 	sta	sp	       	; put it back
 	bcc	addysp_1       	; if no carry, we're done
 	inc	sp+1	       	; inc hi byte
addysp_1:
 	pla		       	; get A back
 	rts


;
;
	.export		subysp		; sub Y from SP
	.export		decsp1, decsp2, decsp3, decsp4
	.export		decsp5, decsp6, decsp7, decsp8

; Do this one by hand, cause it gets used a lot

decsp2:	ldy	sp
       	beq	@L1
       	dey
       	beq	@L2
       	dey
       	sty	sp
       	rts

@L1:	dey
@L2:    dey
       	sty	sp
       	dec	sp+1
	rts

; Decrement by 1 also done as fast as possible

decsp1:	ldy	sp
	bne	*+4
	dec	sp+1
	dec	sp
	rts

decsp3:	ldy	#3
	bne	subysp

decsp4:	ldy	#4
	bne	subysp

decsp5:	ldy	#5
 	bne	subysp

decsp6:	ldy	#6
 	bne	subysp

decsp7:	ldy	#7
 	bne	subysp

decsp8:	ldy	#8
; 	bne	subysp

subysp:	pha			; save A
       	sty    	tmp1		; save the value
	lda	sp    		; get lo byte
	sec
	sbc	tmp1   	       	; sub y val
	sta	sp  	  	; put it back
	bcs	*+4
	dec	sp+1
	pla			; get A back
	rts			; done

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

; ax --> (sp),y


	.export		staxspp		; store AX thru (sp), and pop
staxspp:
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


	.export		staspp		; store A thru (sp), and pop
staspp:
	ldy	#1
	pha
	lda	(sp),y
	sta	ptr1+1
	dey
	lda	(sp),y
	sta	ptr1
	pla
	sta	(ptr1),y
       	jmp	incsp2		; Drop address


;
; Boolean function return entries.
;

	.export		return0, return1

return1:
	ldx	#0
	lda	#1
	rts

return0:
	lda	#0
	tax
	rts

;
; random stuff
;

; (a/x) 16--> (--sp)

    	.export	 	pushwaxi
pushwaxi:      	       		; push word at (ax)
    	jsr    	ldaxi
    	jmp    	pushax

; (a/x) 8--> (--sp)

	.export	 	pushbaxi	; push byte at (ax)
pushbaxi:
	jsr	ldai
	jmp	pushax



