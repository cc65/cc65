;
; Ullrich von Bassewitz, 26.10.2000
;
; Screen size variables
;


	.export		xsize, ysize
       	.constructor	initscrsize

	.include	"pet.inc"

.code

initscrsize:
       	ldx	SCR_LINELEN
	inx			; Variable is one less
       	stx 	xsize
	lda	#25
	sta    	ysize
	rts


.bss

xsize: 	.res	1
ysize:	.res	1



