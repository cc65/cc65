;
; Ullrich von Bassewitz, 02.06.1998
;
; int isprint (int c);
;

	.export		_isprint
	.import		__ctype

_isprint:
	cpx	#$00		; Char range ok?
	bne	@L1		; Jump if no
	tay
	lda	__ctype,y	; Get character classification
	eor	#$10		; NOT a control char
       	and    	#$10		; Mask control char bit
	rts

@L1:	lda	#$00		; Return false
	tax
	rts

