;
; write(iocb, buf, nbytes)->nbytes written
;
	.include "atari.inc"
	.import __rwsetup,__do_oserror,__inviocb,__oserror
	.export	_write
_write:
	jsr	__rwsetup	; do common setup
	beq	write9		; if size 0, it's a no-op
	cpx	#$FF		; invalid iocb?
	beq	_inviocb
	lda	#PUTCHR
	sta	ICCOM,x
	jsr	CIOV
	bpl	write9
	jmp	__do_oserror	; update errno

write9:
	lda	ICBLL,x		; get buf len lo
	pha
	lda	ICBLH,x		; buf len hi
	tax
	lda	#0
	sta	__oserror	; clear system dependend error code
	pla
	rts

_inviocb:
	jmp	__inviocb
