
	.export PLOT

	.include	"pcengine.inc"

PLOT:

	bcs @getpos

	tya
	clc
	adc _plotlo,x
	sta	SCREEN_PTR

	lda _plothi,x
	adc #0
	sta	SCREEN_PTR+1

	;clc
	;adc _colplot,x
	;sta CRAM_PTR

	;lda #$23
	;sta CRAM_PTR+1

@getpos:
	ldx CURS_Y
	ldy CURS_X
	rts

_plotlo:
	.repeat screenrows,line
	.byte <($0000+(line*$80))
	.endrepeat

_plothi:
	.repeat screenrows,line
	.byte >($0000+(line*$80))
	.endrepeat


