;
; Ullrich von Bassewitz, 02.06.1998
;
; int isalpha (int c);
;

	.export		_isalpha
	.import		__ctype

_isalpha:
	tay
	lda	__ctype,y	; Get character classification
	and 	#$03		; Mask character bits
	rts

