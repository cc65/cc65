
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 21.12.1999, 2.1.2003

; char SetGEOSDisk (void);

	    .import setoserror
	    .export _SetGEOSDisk

	    .include "../inc/jumptab.inc"

_SetGEOSDisk:
	jsr SetGEOSDisk
	jmp setoserror
