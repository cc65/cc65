;
; Ullrich von Bassewitz, 26.11.1998
;
; int kbhit (void);
;

	.export		_kbhit
	.import		return0, return1

	.include	"pet.inc"

_kbhit:
	lda	KEY_COUNT   	; Get number of characters
	bne   	L1
	jmp	return0
L1:	jmp	return1




