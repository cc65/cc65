;
; Ullrich von Bassewitz, 02.06.1998
;
; int ispunct (int c);
;

	.export		_ispunct
       	.include	"ctype.inc"

_ispunct:
	cpx	#$00		; Char range ok?
	bne	@L1		; Jump if no
	tay
	lda	__ctype,y	; Get character classification
       	eor    	#CT_NOT_PUNCT	; NOT (space | control | digit | alpha)
       	and    	#CT_NOT_PUNCT	; Mask relevant bits
	rts

@L1:	lda	#$00		; Return false
	tax
	rts

