;
; Ullrich von Bassewitz, 22.09.1998
;
; Low level stuff for screen output/console input
;

   	.export		initconio
	.import		xsize, ysize

	.include	"cbm610.inc"

initconio:
	lda	#80
	sta	xsize
	lda	#25
	sta	ysize
	rts


