
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 30.10.99

; void FillRam         (char what, char *dest, int length);

	    .import DoublePop, popa
	    .export _FillRam

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"

_FillRam:
	    jsr DoublePop
	    jsr popa
	    sta r2L
	    jmp FillRam
