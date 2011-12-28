
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 27.10.2001
; 06.03.2002

; void gotox (unsigned char x);
; void gotoy (unsigned char y);
; void gotoxy (unsigned char x, unsigned char y);

	    .export _gotox, _gotoy, _gotoxy, fixcursor
	    .import popa
	    .importzp cursor_x, cursor_y, cursor_c, cursor_r

	    .include "../inc/jumptab.inc"

_gotox:	    sta cursor_c
	    jmp fixcursor

_gotoy:	    sta cursor_r
	    inc cursor_r
	    jmp fixcursor

_gotoxy:    sta cursor_r
	    inc cursor_r
	    jsr popa
	    sta cursor_c

; convert 8x8 x/y coordinates to GEOS hires
fixcursor:
	    lda cursor_c
	    sta cursor_x
	    lda #0
	    sta cursor_x+1
	    lda cursor_r
	    sta cursor_y
	    ldx #cursor_x
	    ldy #3
	    jsr DShiftLeft
	    asl cursor_y
	    asl cursor_y
	    asl cursor_y
	    rts
