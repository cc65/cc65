
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 27.10.2001

; unsigned char cgetc (void);

	    .export _cgetc

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	    .include "cursor.inc"

_cgetc:
; show cursor if needed
	    lda cursor_flag
	    beq L0

	    lda #1
	    sta r3L
	    lda cursor_x
	    ldx cursor_x+1
	    sta r4L
	    sta stringX
	    stx r4H
	    stx stringX+1
	    lda cursor_y
	    sec
	    sbc curHeight
	    sta r5L
	    sta stringY
	    jsr PosSprite
	    jsr PromptOn

L0:	    jsr GetNextChar
	    tax
	    beq L0
	    pha
	    jsr PromptOff
	    pla
	    ldx #0
	    rts
