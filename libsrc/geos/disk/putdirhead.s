
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; char PutDirHead (void);

	    .import __oserror
	    .export _PutDirHead

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_PutDirHead:
	jsr PutDirHead
	stx __oserror
	txa
	rts
