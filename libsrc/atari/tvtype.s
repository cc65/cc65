;
; Christian Groessler, June 2004
;
; unsigned char _get_tv(void)
;
; returns the TV system the machine is using
; 0 - NTSC
; 1 - PAL
;


	.include	"atari.inc"
	.export		__get_tv

.proc	__get_tv

	lda	PAL		; use hw register, PALNTS is only supported on XL/XE ROM
	ldx	#0
	and	#$0e
	bne	@NTSC
	lda	#1
	rts
@NTSC:	txa
	rts

.endproc
