
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 22.12.99

; char CmpString (char *dest, char* source);

	    .import DoubleSPop
	    .export _CmpString

	    .include "../inc/jumptab.inc"

_CmpString:
	    jsr DoubleSPop
	    jmp CmpString
