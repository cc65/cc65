
;
; Maciej 'YTM/Elysium' Witkowiak
;
; Screen size variables
;
; 6.3.2001

	
	.include 	"../inc/geossym.inc"

	.export		xsize, ysize
	.constructor	initscrsize

.code

initscrsize:
	lda	graphMode
	bpl	L1
	lda	#80		; 80 columns (more or less)
	.byte	$2c
L1:	lda	#40		; 40 columns (more or less)
	sta	xsize
	lda	#24		; something like that for Y size
	sta	ysize
	rts

.bss

xsize: 	.res	1
ysize:	.res	1
