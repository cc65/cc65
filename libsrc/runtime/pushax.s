;
; Ullrich von Bassewitz, 26.10.2000
;
; CC65 runtime: Push value in a/x onto the stack
;

       	.export	    	push0, pusha0, pushax
	.importzp   	sp

push0:	lda	#0
pusha0:	ldx	#0

; This function is used *a lot*, so don't call any subroutines here.
; Beware: The value in ax must not be changed by this function!
; Beware^2: The optimizer knows about the value of Y after the function
;           returns!

.proc	pushax

	pha
	lda	sp
	sec
	sbc	#2
	sta	sp		; (13)
	bcs	@L1
	dec	sp+1
@L1:	ldy	#1
	txa			; (20)
	sta	(sp),y
	pla
	dey
	sta	(sp),y		; (38)
	rts

.endproc
