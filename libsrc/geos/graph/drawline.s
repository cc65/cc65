
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 29.10.99

; void DrawLine         (struct window *mywindow);

	    .import _InitDrawWindow
	    .export _DrawLine

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	    .include "../inc/geosmac.ca65.inc"

_DrawLine:
	    tay
	    PushW r2
	    tya
	    jsr _InitDrawWindow
	    MoveW r2, r11
	    PopW r2
	    jmp DrawLine
