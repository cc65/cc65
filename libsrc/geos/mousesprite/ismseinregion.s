
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 21.12.1999, 2.1.2003

; char IsMseInRegion (struct window *mywindow);

	    .import RectRegs
	    .import return0, return1

	    .export _IsMseInRegion
	    
	    .include "../inc/jumptab.inc"

_IsMseInRegion:
	    jsr RectRegs
	    jsr IsMseInRegion
	    bne L1
	    jmp return0
L1:	    jmp return1
