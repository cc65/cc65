;
; Ullrich von Bassewitz, 06.08.1998
;
; int kbhit (void);
;

	.export		_kbhit
	.import		return0, return1

	.include	"atari.inc"

_kbhit:
	lda	CH	; Get number of characters
	cmp	#$FF
	bne   	L1
	jmp	return1
L1:	jmp	return0




