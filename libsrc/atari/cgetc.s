;
; Christian Groessler, November-2002
;
; get a char from the keyboard
; char cgetc(void)
;

	.include "atari.inc"
	.export _cgetc
	.import	cursor
	
_cgetc:
	jsr	setcursor
	jsr	@1
	ldx	#0
	rts

@1:	lda	KEYBDV+5
	pha
	lda	KEYBDV+4
	pha
	lda	#12
	sta	ICAX1Z		; fix problems with direct call to KEYBDV
	rts


.proc	setcursor

	lda	OLDCHR		; get char at current cursor position
	ldy	#0		; needed later

	ldx	cursor		; current cursor setting as requested by the user
	beq	off
	ldx	#0
	beq	cont

off:	inx
cont:	stx	CRSINH		; update system variable

	beq	turnon
	and	#$7f		; clear high bit / inverse flag
finish:	sta	(OLDADR),y	; update on-screen display
	rts

turnon:	ora	#$80		; set high bit / inverse flag
	bne	finish

.endproc
