
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 27.10.2001

; unsigned char cursor (unsigned char onoff);

	    .export _cursor
	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	    .include "cursor.inc"

_cursor:

	tay			; onoff into Y
	ldx	#0		; High byte of result
	ldx	cursor_flag	; Get old value
	sty	cursor_flag	; Set new value
	tya
	beq	L1
	lda	curHeight	; prepare cursor
	jsr	InitTextPrompt
	lda	cursor_x	; position it on screen
	sta	r4L
	lda	cursor_x+1
	sta	r4H
	lda	cursor_y
	sec
	sbc	curHeight
	sta	r5L
	lda	#1
	sta	r3L
	jsr	PosSprite
L1:	rts
