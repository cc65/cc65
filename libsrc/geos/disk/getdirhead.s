
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; char GetDirHead (void);

	    .export _GetDirHead

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_GetDirHead:
	jsr GetDirHead
	stx errno
	txa
	rts
