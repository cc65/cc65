;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Convert tos from long to int
;

       	.export		tosint
	.import		incsp2
	.importzp	sp

; Convert TOS from long to int by cutting of the high 16bit

tosint:	pha
	ldy    	#0
	lda	(sp),y 	       	; sp+1
	ldy	#2
	sta	(sp),y
	ldy	#1
	lda	(sp),y
	ldy	#3
	sta	(sp),y
	pla
       	jmp    	incsp2	  	; Drop 16 bit

