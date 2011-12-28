
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 22.12.99

; int CRC         (char *memory, int length);

	    .import DoublePop
	    .export _CRC

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"

_CRC:
	    jsr DoublePop
	    jsr CRC
	    lda r2L
	    ldx r2H
	    rts
	    