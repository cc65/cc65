;
; Ullrich von Bassewitz, 02.06.1998
;
; int tolower (int c);
;

	.export		_tolower
	.import		__ctype, __cdiff

_tolower:
       	tay			; Get C into Y
	lda	__ctype,y	; Get character classification
	lsr	a
	lsr	a		; Get bit 1 (upper case char) into carry
	tya			; Get char back into A
       	bcc    	L9		; Jump if no upper case char
   	sbc	__cdiff		; make lower case char (carry already set)
L9:    	rts	  		; CC are set

