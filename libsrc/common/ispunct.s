;
; Ullrich von Bassewitz, 02.06.1998
;
; int ispunct (int c);
;

	.export		_ispunct
	.import		__ctype

_ispunct:
	tay
	lda	__ctype,y	; Get character classification
       	eor    	#$37   	       	; NOT (space | control | digit | char)
       	and    	#$37		; Mask relevant bits
	rts

