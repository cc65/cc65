
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; void LoadCharSet (struct fontdesc *myFont);

	    .export _LoadCharSet

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"

_LoadCharSet:
	    sta r0L
	    stx r0H
	    jmp LoadCharSet
