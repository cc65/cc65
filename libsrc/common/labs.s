;
; Ullrich von Bassewitz, 17.06.1998
;
; long labs (long x);
;

	.export		_labs
	.import		negeax, tsteax
	.importzp	sreg

_labs: 	ldy	sreg+1		; test hi byte
  	bpl	L1
	jmp	negeax 		; Negate if negative
L1:    	rts

