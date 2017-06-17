
		.export _cpeekcol
		.export _cpeekcolxy

		.import _gotoxy

		.include	"c128.inc"

	.segment "CODE"

_cpeekcolxy:

	jsr	_gotoxy		; Set cursor

_cpeekcol:

	bit MODE
	bmi @c80

   	ldy CURS_X
	lda	(CRAM_PTR),y	; get col
	and #$0f
	ldx #0
	rts

@c80:
	lda CRAM_PTR
	ldy CRAM_PTR+1
	clc
	adc CURS_X
	bcc @s
	iny
@s:
	; get byte from vdc mem
	ldx	#VDC_DATA_LO
	stx	VDC_INDEX
@L0:	bit	VDC_INDEX
	bpl	@L0
	sta	VDC_DATA
	dex
	;;tya
	stx	VDC_INDEX
	sty	VDC_DATA

	ldx	#VDC_DATA_RW
	stx	VDC_INDEX
@L1:	bit	VDC_INDEX
	bpl	@L1
	lda	VDC_DATA


	and #$0f

; translate vdc->vic colour

vdctovic:
       	ldy	#16
@L2:    cmp	$CE5C-1,y
        beq	@L3
        dey
        bne     @L2
@L3:	
        dey
		tya

    ldx #0
	rts
