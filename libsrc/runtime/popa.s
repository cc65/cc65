;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Pop a from stack
;

       	.export	  	popa
	.importzp	sp

.proc	popa

.ifpc02
	lda	(sp)
.else
	ldy 	#0
 	lda	(sp),y		; Read byte
.endif
      	inc	sp
       	beq	@L1
	rts

@L1:	inc	sp+1
	rts

.endproc


