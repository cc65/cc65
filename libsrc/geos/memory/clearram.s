
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 30.10.99

; void ClearRam         (char *dest, int length);

	    .import DoublePop
	    .export _ClearRam

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"

_ClearRam:
	    jsr DoublePop
	    jmp ClearRam
