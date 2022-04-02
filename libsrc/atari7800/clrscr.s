
        .include        "atari7800.inc"

        .export _clrscr

	.import	CURS_X
	.import	CURS_Y
	.import	_screen
	.import	pushax, __bzero

	.code

	.proc _clrscr

	lda	#0
	sta	CURS_X
	sta	CURS_Y
	lda	#<(_screen)
	ldx	#>(_screen)
	jsr	pushax
	ldx	#>(charsperline * screenrows)
	lda	#<(charsperline * screenrows)
	jmp	__bzero

	.endproc

;-------------------------------------------------------------------------------
; force the init constructor to be imported

                .import initconio
conio_init      = initconio
