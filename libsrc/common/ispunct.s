;
; Ullrich von Bassewitz, 02.06.1998
;
; int ispunct (int c);
;

	.export		_ispunct
	.import		__ctype

_ispunct:
	cpx	#$00		; Char range ok?
	bne	@L1		; Jump if no
	tay
	lda	__ctype,y	; Get character classification
       	eor    	#$37   	       	; NOT (space | control | digit | char)
       	and    	#$37		; Mask relevant bits
	rts

@L1:	lda	#$00		; Return false
	tax
	rts

