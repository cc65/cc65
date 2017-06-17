
		.export _cpeekcol
		.export _cpeekcolxy

		.import _gotoxy

		.include "c16.inc"

	.segment "CODE"

_cpeekcolxy:

	jsr	_gotoxy		; Set cursor

_cpeekcol:

   	ldy CURS_X
	lda	(CRAM_PTR),y	; get color
	;and #$0f is this ok?
    ldx #0
	rts
