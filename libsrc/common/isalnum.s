;
; Ullrich von Bassewitz, 02.06.1998
;
; int isalnum (int c);
;

	.export		_isalnum
	.import		__ctype

_isalnum:
	tay
	lda	__ctype,y	; Get character classification
	and    	#$07		; Mask character/digit bits
	rts

