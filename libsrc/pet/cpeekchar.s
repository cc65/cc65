
		.export _cpeekchar
		.export _cpeekcharxy

		.import _gotoxy

		.include	"pet.inc"

	.segment "CODE"

_cpeekcharxy:

	jsr	_gotoxy		; Set cursor

_cpeekchar:

   	ldy CURS_X
	lda	(SCREEN_PTR),y	; get char
    ldx #0
	
	and #$7f

	;   0 - $1f +$40
    ; $20 - $3f
	; $40 - $7e +$80

	cmp #$1f
	bcs @sk1
;;	clc
	adc #$40
	rts

@sk1:
    cmp #$40
	bcc @end
	clc
	adc #$80
@end:
	rts
