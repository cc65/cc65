
		.export _cpeekcol
		.export _cpeekcolxy

		.import _gotoxy

		.include	"c64.inc"

	.segment "CODE"

_cpeekcolxy:

	jsr	_gotoxy		; Set cursor

_cpeekcol:

   	ldy #0
	lda	(CRAM_PTR),y	; get char
	and #$0f
    ldx #0
	rts
