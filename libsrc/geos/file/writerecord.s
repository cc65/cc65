
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 25.12.99

; char WriteRecord  (char *buffer, int length);

	    .export _WriteRecord
	    .import popax

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_WriteRecord:
	sta r2L
	stx r2H
	jsr popax
	sta r7L
	stx r7H
	jsr WriteRecord
	stx errno
	txa
	rts
