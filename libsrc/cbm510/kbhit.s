;
; Ullrich von Bassewitz, 06.08.1998
;
; int kbhit (void);
;

	.export		_kbhit
	.import		return0, return1

	.include	"cbm510.inc"

.proc	_kbhit
	lda	KeyIndex      	; Get number of characters
	bne   	L1
	jmp	return0
L1:	jmp	return1
.endproc






