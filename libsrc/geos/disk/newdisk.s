
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; char NewDisk (void);

	    .export _NewDisk

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_NewDisk:
	jsr NewDisk
	stx errno
	txa
	rts
