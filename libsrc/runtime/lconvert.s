;
; Ullrich von Bassewitz, 05.08.1998
;
; CC65 runtime: long conversion routines
;

;
; Convert TOS from long to int by cutting of the high 16bit
;
       	.export		tosint, tosulong, toslong, axulong, axlong
	.import		incsp2,	decsp2
	.importzp	sp, sreg

tosint:	pha
	ldy    	#0
	lda	(sp),y 	       	; sp+1
	ldy	#2
	sta	(sp),y
	ldy	#1
	lda	(sp),y
	ldy	#3
	sta	(sp),y
	pla
       	jmp    	incsp2		; Drop 16 bit

;
; Convert TOS from int to long
;

tosulong:
       	pha
    	jsr	decsp2		; Make room
	ldy    	#2
	lda	(sp),y
	ldy	#0
       	sta	(sp),y
	ldy    	#3
	lda    	(sp),y
	ldy	#1
	sta	(sp),y
	lda    	#0 		; Zero extend
toslong2:
	iny
	sta	(sp),y
	iny
	sta	(sp),y
	pla
	rts

toslong:
       	pha
    	jsr	decsp2		; Make room
	ldy    	#2
	lda	(sp),y
	ldy	#0
       	sta	(sp),y
	ldy    	#3
	lda    	(sp),y
	bmi	toslong1
	ldy	#1
	sta	(sp),y
	lda	#$00		; Positive, high word is zero
	bne	toslong2
toslong1:
	ldy	#1
	sta	(sp),y
	lda	#$FF
	bne	toslong2

;
; Convert AX from int to long in EAX
;

axulong:
	ldy	#0
	sty	sreg
	sty	sreg+1
       	rts

axlong:	cpx    	#$80  		; Positive?
	bcc	axulong		; Yes, handle like unsigned type
	ldy    	#$ff
	sty	sreg
	sty	sreg+1
	rts



