;
; Ullrich von Bassewitz, 31.08.1998
;
; CC65 runtime: Load a unsigned from offset in stack
;

       	.export		ldausp, ldauysp
	.importzp	sp

ldausp:	ldy    	#0
ldauysp:
 	ldx	#0
 	lda	(sp),y
 	rts


