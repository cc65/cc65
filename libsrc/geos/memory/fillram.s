
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 30.10.99, 15.07.2001

; void FillRam         (char *dest, char what, int length);

	    .import popa, popax
	    .export _FillRam

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"

_FillRam:
	    sta r0L
	    stx r0H
	    jsr popa
	    sta r2L
	    jsr popax
	    sta r1L
	    stx r1H
	    jmp FillRam
