;
; Ullrich von Bassewitz, 02.06.1998
;
; int isblank (int c);
;
; cc65 (and GNU) extension.
;

	.export		_isblank
	.import		__ctype

_isblank:
	cpx	#$00		; Char range ok?
	bne	@L1		; Jump if no
	tay
	lda	__ctype,y	; Get character classification
       	and    	#$80		; Mask blank bit
	rts

@L1:	lda	#$00		; Return false
	tax
	rts

