;
; Ullrich von Bassewitz, 17.06.1998
;
; int abs (int x);
;

	.export		_abs
	.import		negax

_abs:	dex
	inx     	     	; test hi byte
	bpl	L1
	jmp	negax	     	; Negate if negative
L1:	rts



