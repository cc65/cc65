;
; Mark Keates, Christian Groessler, Piotr Fusik, Karri Kaksonen
;
; void cputcxy (unsigned char x, unsigned char y, char c);
; void cputc (char c);
;

        .export         _cputc
	.export		_textcolor
        .import         _gotoxy, _gotox, _gotoy, tosaddax, tosumula0, pusha0
	.import		pushax
	.import		_screen
	.import		CURS_X, CURS_Y

        .include        "atari7800.inc"
        .include        "extzp.inc"

	.data
;-----------------------------------------------------------------------------
; Holder of the text colour offset
; 0 = red, 42 = green, 82 = white
;
txtcolor:
	.byte	0

	.code

;-----------------------------------------------------------------------------
; Change the text colour
;
; Logical colour names are
; 0 = red
; 1 = green
; 2 = white
;
; The routine will also return the previous textcolor
;
        .proc   _textcolor

	beq	@L2
	sec
	sbc	#1
	beq	@L1
	lda	#82
	jmp	@L2
@L1:	lda	#42
@L2:	ldy	txtcolor
	sta	txtcolor
	tya
	bne	@L3
	rts
@L3:	sec
	sbc	#42
	bne	@L4
	lda	#1
	rts
@L4:	lda #2
	rts
        .endproc

;-----------------------------------------------------------------------------
; Put a character on screen
;
; The code will handle newlines that wrap to start of screen
;
        .proc   _cputc

	cmp     #$0A            ; LF
        beq     @L3
        cmp	#$20		; ' '
        beq     @L1

        jsr     @L5
@L1:
        lda	CURS_X
	cmp	#(charsperline-1)
        bne     @L2
	jmp	@L3
@L2:
	clc
	adc	#1
	jmp	_gotox

@L3:
	lda	#0
	jsr	_gotox
        lda	CURS_Y
	cmp	#(screenrows-1)
        bne     @L4
	lda	#0
	jmp	_gotoy
@L4:
        clc
	adc	#1
	jmp	_gotoy

@L5:
	cmp	#$3F		; '?'
	bne	@L6
	lda	#$01
	jmp	@L10
@L6:
	cmp	#$7C		; '|'
	bne	@L7
	lda	#$05
	jmp	@L10
@L7:
	cmp	#$41		; >= 'A'
	bcc	@L8
	and	#$5F
	sec
	sbc	#($41 - 16)
	jmp	@L10
@L8:
	sec
	sbc	#($2A)
@L10:
	sec
	adc	txtcolor
	asl
	pha

	lda	CURS_Y
	jsr	pusha0
	lda	#charsperline
	jsr	tosumula0
	clc
	adc	CURS_X
	bcc	@L11
	inx
@L11:	jsr	pushax
	lda     #<(_screen)
	ldx     #>(_screen)
	jsr     tosaddax
	sta	ptr7800
	stx	ptr7800+1

	pla
	ldy	#0
	sta	(ptr7800),y
	rts

        .endproc

