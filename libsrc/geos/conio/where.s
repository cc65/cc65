
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 27.10.2001

; unsigned char wherex (void);
; unsigned char wherey (void);

	    .export _wherex, _wherey
	    .importzp tmp1, tmp2

	    .include "../inc/jumptab.inc"
	    .include "cursor.inc"

_wherex:    lda cursor_x
	    sta tmp1
	    lda cursor_x+1
	    sta tmp2
	    lda #tmp1
	    ldy #3
	    jsr DShiftRight
	    lda tmp1
	    rts

_wherey:    lda cursor_y
	    lsr
	    lsr
	    lsr
	    rts
