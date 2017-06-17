
		.export _cpeekchar
		.export _cpeekcharxy

		.import _gotoxy

		.include	"cbm610.inc"
		.include	"extzp.inc"

	.segment "CODE"

_cpeekcharxy:

	jsr	_gotoxy		; Set cursor

_cpeekchar:
                                
	ldx	IndReg
	ldy	#$0F
	sty	IndReg
       	
	ldy    	CURS_X
	lda	(CharPtr),y 	; get char
	; convert to asc
	and #$7f

	;   0 - $1f +$40
    ; $20 - $3f
	; $40 - $7e +$80

	cmp #$1f
	bcs @sk1
;;	clc
	adc #$40
	jmp @end

@sk1:
    cmp #$40
	bcc @end
	clc
	adc #$80

@end:
	stx	IndReg
	ldx #0
	rts
