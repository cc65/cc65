;
; Ullrich von Bassewitz, 06.08.1998
;
; Low level stuff for screen output/console input
;

     	.export		initconio
	.exportzp	CURS_X, CURS_Y
	.import		xsize, ysize

	.include	"../cbm/cbm.inc"
	.include	"c64.inc"

.code

initconio:
	jsr	SCREEN
	stx	xsize
	sty	ysize
	rts



