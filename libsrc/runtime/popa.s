;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Pop a from stack
;

       	.export	  	popa
	.importzp	sp

.proc	popa

	ldy 	#0
 	lda	(sp),y		; Read byte
	inc	sp
	bne	@L1
	inc	sp+1
@L1:	rts

.endproc


