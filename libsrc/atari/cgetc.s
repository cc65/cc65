;
; Christian Groessler, November-2002
;
; get a char from the keyboard
; char cgetc(void)
;

	.include "atari.inc"
	.export _cgetc,setcursor
	.import	cursor,mul40

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

	ldy	#0
	lda	OLDCHR
	sta	(OLDADR),y

	lda	ROWCRS
	jsr	mul40
	clc
	adc	SAVMSC		; add start of screen memory
	sta	OLDADR
	txa
	adc	SAVMSC+1
	sta	OLDADR+1
	lda	COLCRS
	adc	OLDADR
	sta	OLDADR
	bcc	nc
	inc	OLDADR+1
nc:	lda	(OLDADR),y
	sta	OLDCHR

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
