;
; Ullrich von Bassewitz, 02.06.1998
;
; int isblank (int c);
;
; cc65 (and GNU) extension.
;

	.export		_isblank
	.import		__ctype

_isblank:
	tay
	lda	__ctype,y	; Get character classification
       	and    	#$80		; Mask blank bit
	rts

