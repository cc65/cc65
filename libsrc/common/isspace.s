;
; Ullrich von Bassewitz, 02.06.1998
;
; int isspace (int c);
;

	.export		_isspace
	.import		__ctype

_isspace:
	tay
	lda	__ctype,y	; Get character classification
       	and    	#$60   	       	; Mask space bits
	rts

