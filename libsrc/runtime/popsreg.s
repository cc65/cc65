;
; Ullrich von Bassewitz, 21.08.1998
;
; CC65 runtime: Pop TOS into sreg
;

    	.export		popsreg
	.import		incsp2
       	.importzp	sp, sreg

popsreg:
   	pha	     		; save A
   	ldy	#0
   	lda	(sp),y		; get lo byte
   	sta	sreg 		; store it
   	iny
   	lda	(sp),y		; get hi byte
   	sta	sreg+1		; store it
   	pla	     		; get A back
   	jmp	incsp2		; bump stack and return


