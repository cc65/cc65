
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 22.12.99

; void CmpFString (char length, char *dest, char* source);

	    .import DoubleSPop
	    .import popa
	    .export _CmpFString

	    .include "../inc/jumptab.inc"

_CmpFString:
	    jsr DoubleSPop
	    jsr popa
	    jmp CmpFString
