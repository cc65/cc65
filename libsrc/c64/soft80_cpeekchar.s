
		.export _cpeekchar
		.export _cpeekcharxy

		.import _gotoxy
		.import soft80_hi_charset
		.import soft80_lo_charset

		.include	"c64.inc"

		.macpack longbranch

	.segment "CODE"

readdirect:
;		sei
;		dec $01	;; assumed = $36
;		dec $01	;; assumed = $36
		lda (SCREEN_PTR),y
;		inc $01
;		inc $01
;		cli
		rts

_cpeekcharxy:

		jsr	_gotoxy		; Set cursor

_cpeekchar:
            
;;rts
		sei
		;;dec $01	;; assumed = $36
		;;dec $01	;; assumed = $36
		lda #$34
		sta $01

		lda CURS_X
		and #$01

 		jne @l1a

;;		inc $d020

;;jmp *

		ldx #0
@l2aa:
		ldy #0

;;		stx $d020

    	.repeat 8,line
;;        jsr readdirect
		lda (SCREEN_PTR),y
		and #$f0
		sta $e100,y
		cmp soft80_hi_charset+(line*$80),x
;	cmp #0
		bne @l2b
		.if (line < 7)
    	iny
		.endif
		.endrepeat


@backok:
;inc $d020
;		inc $01
;		inc $01
		lda #$36
		sta $01
		cli
		txa
  ;     sec
;		sbc #$20
		ldx #$00
		rts
@l2b:
;jmp *
		inx
		cpx #$80
		jne @l2aa
@backerr:
	;;	inc $01
;;		inc $01
		lda #$36
		sta $01
		cli
		ldx #0
		txa
 		rts

@l1a:
		ldx #0
@l1aa:
		ldy #0
		.repeat 8,line
;;        jsr readdirect
		lda (SCREEN_PTR),y
		and #$0f
		eor soft80_lo_charset+(line*$80),x
 		bne @l2bb
		.if line < 7
    	iny
		.endif
		.endrepeat
		jmp @backok
@l2bb:
		inx
		cpx #$80
		bne @l1aa
 		jmp @backerr
