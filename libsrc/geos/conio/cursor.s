
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 27.10.2001, 23.12.2002

; unsigned char cursor (unsigned char onoff);

	    .exportzp cursor_x, cursor_y, cursor_flag
	    .exportzp cursor_c, cursor_r
	    .export _cursor
	    .import update_cursor
	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"

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

.zeropage

cursor_x:	.res 2		; Cursor column (word)
cursor_y:	.res 1		; Cursor row
cursor_flag:	.res 1		; Cursor on/off (0-off)

cursor_c:	.res 1		; Cursor column (0-39/79)
cursor_r:	.res 1		; Cursor row    (0-24)
