
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 29.10.99, 15.08.2003

; void DrawLine         (char mode, struct window *mywindow);

	    .import _InitDrawWindow
	    .import popa
	    .importzp tmp1
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
	    jsr popa
	    sta tmp1
	    bit tmp1
	    jmp DrawLine
