
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; void BitmapClip	(char skipl, char skipr, int skipy, struct iconpic *myGfx);

	    .import popa, popax
	    .import BitmapRegs
	    .export _BitmapClip

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"

_BitmapClip:
	    jsr BitmapRegs
	    jsr popax
	    sta r12L
	    stx r12H
	    jsr popa
	    sta r11H
	    jsr popa
	    sta r11L
	    jmp BitmapClip
