
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 25.12.99

; char InsertRecord  (void);

	    .export _InsertRecord

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_InsertRecord:
	jsr InsertRecord
	stx errno
	txa
	rts
