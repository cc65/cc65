;
; Christian Groessler
;
; Low level stuff for screen output/console input
;

     	.export		initconio
	.import		xsize, ysize, plot

	.include	"atari.inc"

.code

initconio:
	ldx	#40
	ldy	#24
	stx	xsize
	sty	ysize
	rts

