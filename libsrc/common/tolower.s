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
	and	#$02		; Is this an upper case char?
	beq	L1		; Jump if no
	tya			; Get char back into A
       	sec
	sbc	__cdiff		; make lower case char
       	rts			; CC are set

L1:	tya			; Get char back into A
	rts			; CC are set

