
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 22.12.99

; void CopyFString (char length, char *dest, char* source);

	    .import DoubleSPop
	    .import popa
	    .export _CopyFString

	    .include "../inc/jumptab.inc"

_CopyFString:
	    jsr DoubleSPop
	    jsr popa
	    jmp CopyFString
