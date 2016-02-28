
		.export _cpeekcol
		.export _cpeekcolxy

		.import _gotoxy

		.include	"c128/c128.inc"

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
	stx	VDC_ADDR_REG
@L0:	bit	VDC_ADDR_REG
	bpl	@L0
	sta	VDC_DATA_REG
	dex
	;;tya
	stx	VDC_ADDR_REG
	sty	VDC_DATA_REG

	ldx	#VDC_DATA_RW
	stx	VDC_ADDR_REG
@L1:	bit	VDC_ADDR_REG
	bpl	@L1
	lda	VDC_DATA_REG


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
