
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; char NewDisk (void);

	    .import __oserror
	    .export _NewDisk

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_NewDisk:
	jsr NewDisk
	stx __oserror
	txa
	rts
