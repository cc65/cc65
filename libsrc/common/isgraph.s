;
; Ullrich von Bassewitz, 02.06.1998
;
; int isgraph (int c);
;

	.export		_isgraph
	.import		__ctype

_isgraph:
	cpx	#$00		; Char range ok?
	bne	@L1		; Jump if no
	tay
	lda	__ctype,y	; Get character classification
	eor	#$30		; NOT control and NOT space
	and 	#$30   	       	; Mask character bits
	rts

@L1:	lda	#$00		; Return false
	tax
	rts

