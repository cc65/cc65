
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; char GetNextChar (void);
;	note that if it returns 0 (FALSE) then no characters are available

	    .export _GetNextChar

	    .include "../inc/jumptab.inc"

_GetNextChar = GetNextChar