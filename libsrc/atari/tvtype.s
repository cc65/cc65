;
; Christian Groessler, November 2001
;
; unsigned char get_tv(void)
;
; returns the TV system the machine is using
; 0 - NTSC
; 1 - PAL
;


	.include	"atari.inc"
	.export		_get_tv

.proc	_get_tv

	lda	PAL		; use hw register, PALNTS is only supported on XL/XE ROM
	ldx	#0
	and	#$0e
	bne	@NTSC
	lda	#1
	rts
@NTSC:	txa
	rts

.endproc
