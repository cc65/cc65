
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; char IsMseInRegion (struct window *mywindow);

	    .import RectRegs

	    .export _IsMseInRegion
	    
	    .include "../inc/jumptab.inc"

_IsMseInRegion:
	    jsr RectRegs
	    jmp IsMseInRegion
