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

pushax:	ldy	sp
       	beq	@L1
       	dey
       	beq	@L2
       	dey
@L0:	sty	sp
.ifpc02
      	sta	(sp)		; 65C02 version - saves 2 cycles and one byte
      	ldy	#1		; get hi index
.else
      	ldy	#0    		; get index
      	sta	(sp),y		; store lo byte
      	iny	      		; bump idx
.endif
      	pha	      		; save it
      	txa	      		; get hi byte
    	sta	(sp),y		; store hi byte
    	pla	    		; get A back
    	rts	    		; done

@L1:	dey
@L2:	dey
    	dec	sp+1
.ifpc02
     	bra	@L0
.else
    	jmp	@L0
.endif


