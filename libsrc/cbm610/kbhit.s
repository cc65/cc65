;
; Ullrich von Bassewitz, 06.08.1998
;
; int kbhit (void);
;

	.export		_kbhit
	.import		return0, return1
        .import         keyidx: zp


.proc	_kbhit
	lda	keyidx          ; Get number of characters
	bne   	L1
	jmp	return0
L1:	jmp	return1
.endproc






