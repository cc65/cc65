;
; Ullrich von Bassewitz, 18.08.1998
;
; int kbhit (void);
;

	.export		_kbhit
	.import		return0, return1

	.include	"c128.inc"

_kbhit:
	lda	KEY_COUNT	; Get number of characters
;	ora	FKEY_COUNT	; Or with number of chars from function keys
	bne   	L1
	jmp	return0
L1:	jmp	return1




