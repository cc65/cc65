
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 27.10.2001
; 06.03.2002

; void cputcxy (unsigned char x, unsigned char y, char c);
; void cputc (char c);

	    .export _cputcxy, _cputc, update_cursor

	    .import _gotoxy, fixcursor
	    .import popa
	    .import xsize,ysize
	    .importzp cursor_x, cursor_y, cursor_c, cursor_r

	    .include "../inc/const.inc"
	    .include "../inc/geossym.inc"
	    .include "../inc/jumptab.inc"

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
	cmp	#CR
	beq	do_cr
	cmp	#LF
	beq	do_lf
	cmp	#$1d
	bne	L00
	ldx	#BACKSPACE
	sec
	bcs	L2
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

L1:	clc
L2:	php
	lda	cursor_x
	sta 	r11L
	lda	cursor_x+1
	sta	r11H
	lda	cursor_y
	sta	r1H
	txa
	jsr	PutChar
	plp
	bcs	update_cursor

	inc	cursor_c
	lda	cursor_c
	cmp	xsize			; hit right margin?
	bne	update_cursor
	lda	#0			; yes - do cr+lf
	sta	cursor_c
do_lf:	inc	cursor_r
	lda	cursor_r
	cmp	ysize			; hit bottom margin?
	bne	update_cursor
	dec	cursor_r		; yes - stay in the last line

update_cursor:
	jsr 	fixcursor
	lda	cursor_x
	sta	r4L
	lda	cursor_x+1
	sta	r4H
	lda	cursor_y
	sec
	sbc	curHeight
	sta	r5L
	lda	#1		; update cursor prompt position
	sta	r3L
	jmp	PosSprite

do_cr:	lda	#0
	sta	cursor_c
	beq	update_cursor

