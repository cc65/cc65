;
; Ullrich von Bassewitz, 31.08.1998
;
; CC65 runtime: Load a from offset in stack
;

       	.export		ldasp, ldaysp
	.importzp	sp

ldasp:	ldy    	#0
ldaysp:	ldx	#0
 	lda	(sp),y
 	bpl    	L9		; Jump if positive
 	dex
L9: 	rts


