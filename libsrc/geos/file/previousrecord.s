
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 25.12.99

; char PreviousRecord  (void);

	    .export _PreviousRecord

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"

_PreviousRecord:
	jsr PreviousRecord
	stx errno
	txa
	rts
