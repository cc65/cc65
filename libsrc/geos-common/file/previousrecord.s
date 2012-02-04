
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 25.12.1999, 2.1.2003

; char PreviousRecord  (void);

	    .import setoserror
	    .export _PreviousRecord

	    .include "jumptab.inc"

_PreviousRecord:
	jsr PreviousRecord
	jmp setoserror
