
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 30.10.99

; void PutDecimal         (char style, int value, char y, int x);

	    .import popa, popax
	    .import getintcharint
	    .export _PutDecimal

	    .include "../inc/jumptab.inc"

_PutDecimal:
	    jsr getintcharint
	    jsr popa
	    jmp PutDecimal
