;
; Maciej 'YTM/Elysium' Witkowiak
;
; 27.10.2001
; 06.03.2002

; unsigned char cgetc (void);

	    .export _cgetc
	    .import update_cursor
	    .importzp cursor_x, cursor_y, cursor_flag

	    .include "jumptab.inc"
	    .include "geossym.inc"

_cgetc:
; show cursor if needed
	lda cursor_flag
	beq L0

	jsr update_cursor
	lda cursor_x
	ldx cursor_x+1
	sta stringX
	stx stringX+1
	lda cursor_y
	sec
	sbc curHeight
	sta stringY
	jsr PromptOn

L0:	jsr GetNextChar
	tax
	beq L0
	pha
	jsr PromptOff
	pla
	ldx #0
	rts
