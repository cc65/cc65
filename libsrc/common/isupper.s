;
; Ullrich von Bassewitz, 02.06.1998
;
; int isupper (int c);
;

	.export		_isupper
	.import		__ctype

_isupper:
	cpx	#$00		; Char range ok?
	bne	@L1		; Jump if no
	tay
	lda	__ctype,y	; Get character classification
	and    	#$02   	       	; Mask upper char bit
	rts

@L1:	lda	#$00		; Return false
	tax
	rts

