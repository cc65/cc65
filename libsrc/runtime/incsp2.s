;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Increment the stackpointer by 2. For performance reasons,
; 		this modules does also contain the popax function.

       	.export		popax, incsp2
	.importzp	sp

; Pop a/x from stack. This function will run directly into incsp2

.proc	popax

	ldy  	#1
   	lda	(sp),y		; get hi byte
       	tax	     		; into x
.ifpc02	
	lda	(sp)		; get lo byte
.else
   	dey
   	lda	(sp),y		; get lo byte
.endif

.endproc



.proc	incsp2

	ldy	sp		; 3
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

.endproc





