
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 27.10.2001

; void cputsxy (unsigned char x, unsigned char y, char* s);
; void cputs (char* s);

	    .export _cputsxy, _cputs

	    .import update_cursor, _gotoxy
	    .import popa

	    .include "../inc/const.inc"
	    .include "../inc/geossym.inc"
	    .include "../inc/jumptab.inc"
	    .include "cursor.inc"

_cputsxy:
	sta	r0L		; Save s for later
	stx	r0H
	jsr	popa		; Get Y
	jsr	_gotoxy		; Set cursor, pop x
   	jmp	L0		; Same as cputs...

_cputs:	sta	r0L		; Save s
   	stx	r0H
L0:	ldy	#0
	lda	(r0),y
   	bne	L1		; Jump if there's something
	rts

L1:	lda	cursor_x
	sta 	r11L
	lda	cursor_x+1
	sta	r11H
	lda	cursor_y
	sta	r1H
	jsr	PutString
	jmp	update_cursor
