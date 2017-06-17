
		.export _cpeekcol
		.export _cpeekcolxy

		.import _gotoxy

		.include	"cbm510.inc"
		.include	"extzp.inc"

	.segment "CODE"

_cpeekcolxy:

	jsr	_gotoxy		; Set cursor

_cpeekcol:
     
	lda #0
    ldx #0
	rts
          
	;; why the HELL doesnt this work ?!?
	lda	#$0F
   	ldy CURS_X
	sei
	ldx	IndReg
	sta	IndReg
	lda	(CRAM_PTR),y	; get color
	stx	IndReg
	cli
    ldx #0
	rts
