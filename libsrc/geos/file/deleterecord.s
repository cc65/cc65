
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 25.12.99

; char DeleteRecord  (void);

	    .export _DeleteRecord

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_DeleteRecord:
	jsr DeleteRecord
	stx errno
	txa
	rts
