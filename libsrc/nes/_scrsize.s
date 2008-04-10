;
; Ullrich von Bassewitz, 2003-04-14
; Stefan Haubenthal, 2007-01-21
;
; Screen size function
;

	.export		screensize

	.include	"nes.inc"
	.include	"get_tv.inc"


.proc	screensize

	jsr	_get_tv
	ldx	#charsperline
	ldy	#screenrows
	cmp	#TV::NTSC
	beq	nopal
	dey
nopal:	rts

.endproc
