;
; Ullrich von Bassewitz, 02.06.1998
;
; int isupper (int c);
;

	.export		_isupper
	.import		__ctype

_isupper:
	tay
	lda	__ctype,y	; Get character classification
	and    	#$02   	       	; Mask upper char bit
	rts

