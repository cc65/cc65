;
; Ullrich von Bassewitz, 02.06.1998
;
; int toupper (int c);
;

	.export		_toupper
	.import		__ctype, __cdiff

_toupper:
	tay			; Get c into Y
	lda	__ctype,y	; Get character classification
	lsr	a		; Get bit 0 (lower char) into carry
	tya			; Get C back into A
       	bcc    	L9		; Jump if not lower char
       	clc
	adc	__cdiff		; make upper case char
L9:	rts	  		; CC are set

