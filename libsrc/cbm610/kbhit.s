;
; Ullrich von Bassewitz, 2003-12-20
;
; int kbhit (void);
;

	.export		_kbhit

        .importzp       sysp0
	.import		return0, return1

	.include	"cbm610.inc"

.proc	_kbhit
        ldx     IndReg
        lda     #$0F
        sta     IndReg
        ldy     #$D1            ; Number of keys in kbd buffer
        lda     (sysp0),y
        stx     IndReg
	bne   	L1
	jmp	return0
L1:	jmp	return1
.endproc






