;
; Christian Groessler, 19-Feb-2000
;
; int kbhit (void);
;

	.export		_kbhit
	.import		return0, return1

	.include	"atari.inc"

_kbhit:
	ldx	CH		; last pressed key
	inx			; 255 means "no key"
	bne	L1
	jmp	return0
L1:	jmp	return1
