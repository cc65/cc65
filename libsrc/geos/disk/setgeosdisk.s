
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; char SetGEOSDisk (void);

	    .export _SetGEOSDisk

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"

_SetGEOSDisk:
	jsr SetGEOSDisk
	stx errno
	txa
	rts
