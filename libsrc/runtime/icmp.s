;
; Ullrich von Bassewitz, 10.12.1998
;
; Integer compare function - used by the compare operators
;

	.export		tosicmp
	.importzp	sp, sreg


tosicmp:
  	sta	sreg
	stx	sreg+1	   	; Save ax

	ldy    	#$01
       	lda    	(sp),y 	       	; Get high byte
	tax
    	dey
    	lda	(sp),y	   	; Get low byte

; Inline incsp2 for better performance

    	inc	sp	   	; 5
       	bne	@L1	   	; 3
    	inc	sp+1	   	; (5)
@L1:	inc	sp	   	; 5
    	bne	@L2	   	; 3
    	inc	sp+1	   	; (5)

; Do the compare.

@L2:	cpx	sreg+1	   	; Compare high byte
    	bne    	@L3
    	cmp	sreg  	   	; Compare low byte
       	beq	@L3
       	bcs    	@L4
       	lda	#$FF		; Set the N flag
@L3: 	rts

@L4:   	lda    	#$01		; Clear the N flag
	rts


