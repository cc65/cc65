;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Decrement the stackpointer by value in y
;

       	.export	  	subysp
	.importzp	sp, tmp1

.proc	subysp

       	sty    	tmp1		; Save the value
	lda	sp    		; Get lo byte
	sec
	sbc	tmp1   	       	; Subtract y value
	sta	sp  	  	; Put result back
	bcs	@L1
	dec	sp+1
@L1:   	rts			; Done

.endproc





