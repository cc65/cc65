;
; Stefan Haubenthal, Jul 12 2003
;
; unsigned char get_ostype(void)
;
; $23 ProDOS 2.0.3
;

	.export		_get_ostype

.proc	_get_ostype

	lda	#0
	ldx	$bf00
	cpx	#$4c	; JMP opcode
	bne	nopdos
	lda	$bfff
nopdos:	rts

.endproc
