
		.export _cpeekchar
		.export _cpeekcharxy

		.import _gotoxy

		.import plot,popa


		.include	"zeropage.inc"
		.include	"c128/c128.inc"

	.segment "CODE"

_cpeekcharxy:

	jsr	_gotoxy		; Set cursor

_cpeekchar:

	lda MODE
	bmi @c80

   	ldy CURS_X
	lda (SCREEN_PTR),y	; get char

@return:
	; convert to asc
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
    ldx #0
	rts

@c80:
	lda SCREEN_PTR
	ldy SCREEN_PTR+1
	clc
	adc CURS_X
	bcc @s
	iny
@s:
	; get byte from vdc mem
	ldx	#VDC_DATA_LO
	stx	VDC_ADDR_REG
@L0:	bit	VDC_ADDR_REG
	bpl	@L0
	sta	VDC_DATA_REG
	dex
	tya
	stx	VDC_ADDR_REG
	sta	VDC_DATA_REG

	ldx	#VDC_DATA_RW
	stx	VDC_ADDR_REG
@L1:	bit	VDC_ADDR_REG
	bpl	@L1
	lda	VDC_DATA_REG
    jmp @return
