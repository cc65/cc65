
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; char OpenDisk (void);

	    .import __oserror
	    .export _OpenDisk

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_OpenDisk:
	jsr OpenDisk
	stx __oserror
	txa
	rts
