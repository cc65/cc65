
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 27.10.2001

; void cputcxy (unsigned char x, unsigned char y, char c);
; void cputc (char c);

	    .export _cputcxy, _cputc, update_cursor

	    .import _gotoxy
	    .import popa

	    .include "../inc/const.inc"
	    .include "../inc/geossym.inc"
	    .include "../inc/jumptab.inc"
	    .include "cursor.inc"

_cputcxy:
	pha	    		; Save C
	jsr	popa		; Get Y
	jsr	_gotoxy		; Set cursor, drop x
	pla			; Restore C

; Plot a character - also used as internal function

_cputc:
	tax			; save character
; some characters are not safe for PutChar
	cmp	#$20
	bcs	L1
	cmp	#$1d
	bne	L00
	ldx	#BACKSPACE
	bne	L1
L00:	cmp	#ESC_GRAPHICS
	beq	L0
	cmp	#ESC_RULER
	beq	L0
	cmp	#GOTOX
	beq	L0
	cmp	#GOTOY
	beq	L0
	cmp	#GOTOXY
	beq	L0
	cmp	#NEWCARDSET
	beq	L0
	cmp	#$1e
	bne	L1
L0:	rts

L1:	lda	cursor_x
	sta 	r11L
	lda	cursor_x+1
	sta	r11H
	lda	cursor_y
	sta	r1H
	txa
	jsr	PutChar

update_cursor:
	lda	r11L
	sta	cursor_x
	sta	r4L
	lda	r11H
	sta	cursor_x+1
	sta	r4H
	lda	r1H
	sta	cursor_y
	sec
	sbc 	curHeight
	sta	r5L
	lda	#1		; update cursor prompt position
	sta	r3L
	jmp	PosSprite
