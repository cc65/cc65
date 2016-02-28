
		.export _cpeekchar
		.export _cpeekcharxy

		.import _gotoxy

		.include	"cbm510/cbm510.inc"

	.segment "CODE"

_cpeekcharxy:

	jsr	_gotoxy		; Set cursor

_cpeekchar:
	lda #0
	ldx #0
	rts
    
	;; ?!?!

	ldx	IndReg
	ldy	#$0F
	sty	IndReg
       	
	ldy    	CURS_X
	lda	(SCREEN_PTR),y 	; get char

	stx	IndReg

	ldx #0
	rts
