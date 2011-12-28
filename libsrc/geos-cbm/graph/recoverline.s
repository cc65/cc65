
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 29.10.99

; void RecoverLine    (char y, int xstart, int xend);

	    .import HLineRegs

	    .export _RecoverLine
	    
	    .include "../inc/jumptab.inc"

_RecoverLine:
	    jsr HLineRegs
	    jmp RecoverLine

