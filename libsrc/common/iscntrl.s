;
; Ullrich von Bassewitz, 02.06.1998
;
; int iscntrl (int c);
;

	.export		_iscntrl
	.import		__ctype

_iscntrl:
	tay
	lda	__ctype,y	; Get character classification
	and    	#$10		; Mask control character bit
	rts


