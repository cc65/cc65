;
; Ullrich von Bassewitz, 05.08.1998
;
; CC65 runtime: Push ints onto the stack
;

;
; push/pop things on stack
;
  	.export	      	push0, push1, push2, push3, push4, push5
	.export	      	push6, push7, pusha0, pushaFF, pushax
	.export	        pusha, pushaysp, pushc0, pushc1, pushc2
	.importzp	sp

pushaFF:
	ldx	#$FF
	bne	pushax

push7:	lda	#7
	bne	pusha0
push6:	lda	#6
	bne	pusha0
push5:	lda	#5
	bne	pusha0
push4:	lda	#4
	bne	pusha0
push3:	lda	#3
	bne	pusha0
push2:	lda	#2
	bne	pusha0
push1:	lda	#1
	bne	pusha0
push0:	lda	#0
;	beq	pusha0
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
	ldy	#0    		; get index
    	sta	(sp),y		; store lo byte
    	pha	      		; save it
    	txa	      		; get hi byte
    	iny	      		; bump idx
    	sta	(sp),y		; store hi byte
    	pla			; get A back
    	rts	    		; done

@L1:	dey
@L2:	dey
    	dec	sp+1
    	bne	@L0		; Branch always

; Push for chars, same warning as above: The optimizer expects the value
; 0 in the Y register after this function.

pushc2:	lda	#2
    	bne	pusha
pushc1:	lda	#1
    	bne	pusha
pushc0:	lda	#0
	beq	pusha
pushaysp:	     
	lda	(sp),y
pusha:	ldy	sp
       	beq	@L1
 	dec	sp
    	ldy	#0
    	sta	(sp),y
    	rts

@L1:	dec	sp+1
    	dec	sp
    	sta	(sp),y
    	rts
