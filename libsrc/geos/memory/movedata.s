
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 30.10.99

; void MoveData         (char* source, char *dest, int length);

	    .import popax
	    .export _MoveData

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"

_MoveData:
	    sta r2L
	    stx r2H
	    jsr popax
	    sta r1L
	    stx r1H
	    jsr popax
	    sta r0L
	    stx r0H
	    jmp MoveData
