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
	.importzp   	sp, sreg

tossubeax:
      	ldy	#0
       	sec
	eor	#$FF
       	adc	(sp),y		; byte 0
       	pha			; Save low byte
	iny
	txa
	eor	#$FF
	adc	(sp),y		; byte 1
	tax
	iny
	lda	(sp),y
	sbc	sreg	      	; byte 2
	sta	sreg
	iny
	lda	(sp),y
	sbc	sreg+1	      	; byte 3
	sta	sreg+1
	pla	      		; Restore byte 0
       	jmp    	addysp1	      	; Drop TOS

