;
; Ullrich von Bassewitz, 26.10.2000
;
; CC65 runtime: Push value in a onto the stack
;

       	.export	  	pushaysp, pusha
	.importzp	sp

; Beware: The optimizer knows about this function!

pushaysp:
	lda	(sp),y
pusha:	ldy	sp
       	beq	@L1
 	dec	sp
.ifpc02
	sta	(sp)
.else
    	ldy	#0
    	sta	(sp),y
.endif
    	rts

@L1:	dec	sp+1
    	dec	sp
    	sta	(sp),y
    	rts

