
;
; Maciej 'YTM/Elysium' Witkowiak
;
; Screen size variables
;
; 6.3.2001

	
	.include 	"../inc/geossym.inc"

	.export		xsize, ysize
	.importzp	cursor_r, cursor_c
	.import		_cursor
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
	ldx	#1
	stx	cursor_r
	dex
	stx	cursor_c
	txa
	jmp	_cursor		; home and update cursor

.bss

xsize: 	.res	1
ysize:	.res	1
