
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 27.10.2001, 23.12.2002

; unsigned char cursor (unsigned char onoff);

	    .export _cursor
	    .import update_cursor
	    .importzp cursor_flag
	    .include "jumptab.inc"
	    .include "geossym.inc"

_cursor:

	tay			; onoff into Y
	ldx	#0		; High byte of result
	lda	cursor_flag	; Get old value
	pha
	sty	cursor_flag	; Set new value
	tya
	beq	L1
	lda	curHeight	; prepare cursor
	jsr	InitTextPrompt
	jsr	update_cursor	; place it on screen
L1:	pla
	rts
