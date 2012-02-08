;
; Maciej 'YTM/Elysium' Witkowiak
;
; 21.12.99, 2.1.2003

; char PutDirHead (void);

	    .import setoserror
	    .export _PutDirHead

	    .include "jumptab.inc"
	
_PutDirHead:
	jsr PutDirHead
	jmp setoserror
