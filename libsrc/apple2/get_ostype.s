;
; Stefan Haubenthal, Jul 12 2003
;
; unsigned char get_ostype(void)
;

	.constructor	initostype
	.export		_get_ostype, ostype

; Identify machine according to:
; "Apple II Miscellaneous TechNote #7: Apple II Family Identification"

initostype:
	ldx	#$01		; Start out with a plain Apple ][
	lda	$FBB3
	cmp	#$38
	beq	identified
	inx			; It's at least an Apple ][+
	cmp	#$EA
	bne	apple2e
	lda	$FB1E
	cmp	#$AD
	beq	identified
	inx			; It's an Apple /// in emulation mode
	bne	identified	; Branch always
apple2e:
	ldx	#$04		; It's at least an Apple //e
	lda	$FBC0
	cmp	#$EA
	beq	identified
	inx			; It's at least an enhanced Apple //e
	cmp	#$E0
	bne	apple2c
	lda	$FBDD
	cmp	#$02
	bne	identified
	inx			; It's an Apple //e Option Card
	bne	identified	; Branch always
apple2c:
	ldx	#$07		; It's an Apple //c
identified:
	stx	ostype
	rts

_get_ostype:
	lda	ostype
	ldx	#$00
	rts

	.bss

ostype:	.res	1
