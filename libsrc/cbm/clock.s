;
; Ullrich von Bassewitz, 21.09.1998
;
; clock_t clock (void);
;

      	.export	       	_clock
	.importzp	sreg

	.include	"cbm.inc"


.proc	_clock

	lda	#0  	    	; Byte 3 is always zero
       	sta    	sreg+1
	jsr	RDTIM
	sty	sreg
	rts	  		; Don't set CC, this has no meaning here

.endproc

