;
; Ullrich von Bassewitz, 02.06.1998
;
; int isdigit (int c);
;

	.export		_isdigit
	.import		__ctype

_isdigit:
	tay
	lda	__ctype,y	; Get character classification
	and    	#$04		; Mask digit bit
	rts

