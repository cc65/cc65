
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; void StashRAM (char REUBank, int length, char *dest, char *from);

	    .import REURegs
	    .export _StashRAM

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"

_StashRAM:
	    jsr REURegs
	    jmp StashRAM
