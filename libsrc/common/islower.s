;
; Ullrich von Bassewitz, 02.06.1998
;
; int islower (int c);
;

	.export		_islower
	.import		__ctype

_islower:
	tay
	lda	__ctype,y	; Get character classification
	and    	#$01		; Mask lower char bit
	rts

