
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 29.10.99, 16.8.2003

; void DrawPoint	(char mode, struct pixel *mypixel);


	    .import PointRegs
	    .import popa
	    .importzp tmp1
	    .export _DrawPoint

	    .include "../inc/jumptab.inc"

_DrawPoint:
	    jsr PointRegs
	    jsr popa
	    sta tmp1
	    bit tmp1
	    jmp DrawPoint
