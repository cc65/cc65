
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; void FetchRAM (char REUBank, int length, char *from, char *dest);

	    .import REURegs
	    .export _FetchRAM

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"

_FetchRAM:
	    jsr REURegs
	    jmp FetchRAM
