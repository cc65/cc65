
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 27.10.2001
; 05.03.2002

; void cpputsxy (unsigned char x, unsigned char y, char* s);
; void cpputs (char* s);

; same as cputsxy and cputs but faster and use proportional font spacing
; does not update cursor position

	    .export _cpputsxy, _cpputs

	    .import _gotoxy
	    .import popa
	    .importzp cursor_x, cursor_y

	    .include "const.inc"
	    .include "geossym.inc"
	    .include "jumptab.inc"

_cpputsxy:
	sta	r0L		; Save s for later
	stx	r0H
	jsr	popa		; Get Y
	jsr	_gotoxy		; Set cursor, pop x
   	jmp	L0		; Same as cputs...

_cpputs:
	sta	r0L		; Save s
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
	jmp	PutString
