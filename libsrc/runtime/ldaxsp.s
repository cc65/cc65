;
; Ullrich von Bassewitz, 31.08.1998
;
; CC65 runtime: Load ax from offset in stack
;

       	.export		ldax0sp, ldaxysp
	.importzp	sp

; Beware: The optimizer knows about the value in Y after return!

ldax0sp:
	ldy	#1
ldaxysp:
	lda	(sp),y		; get high byte
	tax			; and save it
       	bne    	L1		; Try to generate FAST code
   	dey	      		; point to lo byte
   	lda	(sp),y		; load low byte
	rts

L1:	php			; Save Z flag
  	dey
  	lda	(sp),y
  	plp
  	rts


