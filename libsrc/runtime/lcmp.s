;
; Ullrich von Bassewitz, 10.12.1998
;
; Long int compare function - used by the compare operators
;

 	.export		toslcmp
 	.import		incsp4
 	.importzp	sp, sreg, ptr1


toslcmp:
  	sta    	ptr1
  	stx	ptr1+1 	  	; EAX now in sreg:ptr1

  	ldy    	#$03
       	lda    	(sp),y
  	cmp	sreg+1
  	bne	L4

  	dey
  	lda	(sp),y
  	cmp	sreg
  	bne	L1

  	dey
  	lda	(sp),y
  	cmp	ptr1+1
  	bne	L1

  	dey
  	lda	(sp),y
  	cmp	ptr1

L1:	php	      		; Save flags
   	jsr	incsp4		; Drop TOS
   	plp	  		; Restore the flags
   	beq	L2
   	bcs	L3
   	lda	#$FF		; Set the N flag
L2:  	rts

L3:	lda	#$01		; Clear the N flag
  	rts

L4:	php	  		; Save flags
	jsr	incsp4		; Drop TOS
	plp	  		; Restore flags
  	rts


