;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Store a/x indirect into address at top of stack
;

       	.export		staxspp
	.import		incsp2
	.importzp	sp, ptr1

.proc	staxspp

.ifpc02
	pha
      	lda    	(sp)
      	sta    	ptr1
      	ldy    	#1
      	lda    	(sp),y
      	sta    	ptr1+1
      	txa
      	sta     (ptr1),y
      	pla
      	sta    	(ptr1)
.else
      	pha
      	ldy    	#0
      	lda    	(sp),y
      	sta    	ptr1
      	iny
      	lda    	(sp),y
      	sta    	ptr1+1
      	txa
      	sta     (ptr1),y
      	pla
      	dey
      	sta    	(ptr1),y
.endif
	jmp	incsp2		; Drop address

.endproc


