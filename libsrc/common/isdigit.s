;
; Ullrich von Bassewitz, 02.06.1998
;
; int isdigit (int c);
;

	.export		_isdigit
	.import		__ctype

_isdigit:
	cpx	#$00		; Char range ok?
	bne	@L1		; Jump if no
	tay
	lda	__ctype,y	; Get character classification
	and    	#$04		; Mask digit bit
	rts

@L1:	lda	#$00		; Return false
	tax
	rts

