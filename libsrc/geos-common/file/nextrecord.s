;
; Maciej 'YTM/Elysium' Witkowiak
;
; 25.12.1999, 2.1.2003

; char NextRecord  (void);

	    .import setoserror
	    .export _NextRecord

	    .include "jumptab.inc"

_NextRecord:
	jsr NextRecord
	jmp setoserror
