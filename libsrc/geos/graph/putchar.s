
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 30.10.99

; void PutChar         (char character, char y, int x);

	    .import popa
	    .export _PutChar

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"

_PutChar:
	    sta r11L
	    stx r11H
	    jsr popa
	    sta r1H
	    jsr popa
	    jmp PutChar
