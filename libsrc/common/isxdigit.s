;
; Ullrich von Bassewitz, 02.06.1998
;
; int isxdigit (int c);
;

	.export		_isxdigit
	.import		__ctype

_isxdigit:
	tay
	lda	__ctype,y	; Get character classification
       	and    	#$08   	       	; Mask xdigit bit
	rts

