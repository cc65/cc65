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
	ldy 	#0              ; (2)
 	lda	(sp),y		; (7) Read byte
.endif
      	inc	sp              ; (12)
       	beq	@L1             ; (14)
	rts                     ; (20)

@L1:	inc	sp+1
	rts

.endproc


