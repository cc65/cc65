;
; Mark Keates, Christian Groessler, Piotr Fusik, Karri Kaksonen
;
; void cputcxy (unsigned char x, unsigned char y, char c);
; void cputc (char c);
;

        .export         _mono_cputc
        .import         _mono_gotoxy, _mono_gotox, _mono_gotoy, pusha0
	.import		pushax
	.import		_mono_screen

        .include        "atari7800.inc"
        .include        "extzp.inc"

	.code

;---------------------------------------------------------------------------
; 8x16 routine

umula0:
        ldy     #8                 ; Number of bits
	lda	#0
        lsr     ptr7800            ; Get first bit into carry
@L0:    bcc     @L1

        clc
        adc     ptrtmp
        tax
        lda     ptrtmp+1           ; hi byte of left op
	clc
        adc     ptr7800+1
        sta     ptr7800+1
        txa

@L1:    ror     ptr7800+1
        ror     a
        ror     ptr7800
        dey
        bne     @L0
        tax
        lda     ptr7800            ; Load the result
        rts

;-----------------------------------------------------------------------------
; Put a character on screen
;
; The code will handle newlines that wrap to start of screen
;
        .proc   _mono_cputc

	cmp     #$0A            ; LF
        bne     @L4
@L1:	lda	#0		; newline
	jsr	_mono_gotox
        lda	CURS_Y
	cmp	#(screenrows-1)
        bne     @L2
	lda	#0
	beq	@L3
@L2:	clc
	adc	#1
@L3:	jmp	_mono_gotoy

@L4:
	pha

	lda	#0
	sta	ptr7800+1
	sta	ptrtmp+1
	lda	CURS_Y		; Find position on screen buffer
	sta	ptr7800
	lda	#mono_charsperline
	sta	ptrtmp
	jsr	umula0
	clc
	adc	CURS_X
	bcc	@L11
	inx
@L11:	clc
	adc	#<(_mono_screen)
	sta	ptr7800
	bcc	@L12
	inx
@L12:	txa
	clc
	adc	#>(_mono_screen)
	sta	ptr7800+1

	pla			; Print character on screen
	ldy	#0
	sta	(ptr7800),y

        lda	CURS_X		; Increment cursor
	cmp	#(mono_charsperline-1)
        beq	@L1
	clc
	adc	#1
	jmp	_mono_gotox

        .endproc

