;
; Ullrich von Bassewitz, 02.06.1998
;
; int isxdigit (int c);
;

	.export		_isxdigit
	.import		__ctype

_isxdigit:
	cpx	#$00		; Char range ok?
	bne	@L1		; Jump if no
	tay
	lda	__ctype,y	; Get character classification
       	and    	#$08   	       	; Mask xdigit bit
	rts

@L1:	lda	#$00		; Return false
	tax
	rts

