
		.export _cpeekchar
		.export _cpeekcharxy

		.import _gotoxy
		.import mul40
		.importzp ptr4

		.include	"atari/atari.inc"

	.segment "CODE"

_cpeekcharxy:

	jsr	_gotoxy		; Set cursor

_cpeekchar:

	lda	ROWCRS
	jsr	mul40		; destroys tmp4
	clc
	adc	SAVMSC		; add start of screen memory
	sta	ptr4
	txa
	adc	SAVMSC+1
	sta	ptr4+1

   	ldy COLCRS
	lda	(ptr4),y	; get char
	tax

	;; convert to asc

	;; ugly hack here to make tetris fx work :=P
	lda #' '
	cpx #0
    beq @l
	lda #0
@l:
    ldx #0
	rts
