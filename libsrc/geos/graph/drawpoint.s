
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 29.10.99

; void DrawPoint	(struct pixel *mypixel);


	    .import PointRegs
	    .export _DrawPoint

	    .include "../inc/jumptab.inc"

_DrawPoint:
	    jsr PointRegs
	    jmp DrawPoint
