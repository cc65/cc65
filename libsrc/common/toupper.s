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
	and	#$01		; Mask lower char bit
	beq	L1		; Jump if not lower char
	tya			; Get C back into A
       	clc
	adc	__cdiff		; make upper case char
	rts			; CC are set

L1:	tya	     		; Get C back
	rts			; CC are set

