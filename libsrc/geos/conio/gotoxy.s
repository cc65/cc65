
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 27.10.2001

; void gotox (unsigned char x);
; void gotoy (unsigned char y);
; void gotoxy (unsigned char x, unsigned char y);

	    .export _gotox, _gotoy, _gotoxy
	    .import popa

	    .include "../inc/jumptab.inc"
	    .include "cursor.inc"

_gotox:	    sta cursor_x
	    jmp fixcursor

_gotoy:	    sta cursor_y
	    jmp fixcursor

_gotoxy:    sta cursor_y
	    jsr popa
	    sta cursor_x

; convert 8x8 x/y coordinates to GEOS hires
fixcursor:
	    ldx #cursor_x
	    ldy #3
	    jsr DShiftLeft
	    asl cursor_y
	    asl cursor_y
	    asl cursor_y
	    rts
