;
; Christian Groessler, December 2000
;
; unsigned char get_tv(void)
;
; returns the TV system the machine is using
;


	.include	"atari.inc"
	.export		_get_tv

.proc	_get_tv

	lda	PALNTS
	ldx	#0
	rts

.endproc
