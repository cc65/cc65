
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; char GetDirHead (void);

	    .import __oserror
	    .export _GetDirHead

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_GetDirHead:
	jsr GetDirHead
	stx __oserror
	txa
	rts
