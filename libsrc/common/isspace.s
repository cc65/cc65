;
; Ullrich von Bassewitz, 02.06.1998
;
; int isspace (int c);
;

	.export		_isspace
	.import		__ctype

_isspace:
	cpx	#$00		; Char range ok?
	bne	@L1		; Jump if no
	tay
	lda	__ctype,y	; Get character classification
       	and    	#$60   	       	; Mask space bits
	rts

@L1:	lda	#$00		; Return false
	tax
	rts

