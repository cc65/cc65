;
; Ullrich von Bassewitz, 26.11.1998
;
; Low level stuff for screen output/console input
;

     	.export		initconio
	.import		xsize, ysize
	.exportzp	CURS_X, CURS_Y

	.include	"pet.inc"

.code

initconio:
       	ldx	SCR_LINELEN
	inx			; Variable is one less
       	stx 	xsize
	lda	#25
	sta    	ysize
	rts



