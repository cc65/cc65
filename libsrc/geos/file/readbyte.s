
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 25.12.99

; char ReadByte  (void);

	    .export _ReadByte

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"

_ReadByte:
	jsr ReadByte
	stx errno
	rts
