;
; Ullrich von Bassewitz, 02.06.1998
;
; int isalpha (int c);
;

	.export		_isalpha
	.import		__ctype

_isalpha:
	cpx	#$00		; Char range ok?
	bne	@L1		; Jump if no
	tay
	lda	__ctype,y	; Get character classification
	and 	#$03		; Mask character bits
	rts

@L1:	lda	#$00		; Return false
	tax
	rts

