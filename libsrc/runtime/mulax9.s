;
; Ullrich von Bassewitz, 16.03.2002
;
; CC65 runtime: Multiply the primary register
;

   	.export	    	mulax9
	.importzp      	ptr1, tmp1

.proc	mulax9

	sta   	ptr1
	stx   	ptr1+1
	stx   	tmp1
	asl   	a
	rol   	tmp1	    		; * 2
	asl   	a
	rol   	tmp1			; * 4
	asl   	a
	rol   	tmp1			; * 8
	clc
	adc   	ptr1			; * (8+1)
	pha
	lda	tmp1
	adc	ptr1+1
	tax
	pla
	rts

.endproc


