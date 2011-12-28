
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 30.10.1999, 2.1.2003

; char GetRandom (void);

	    .export _GetRandom

	    .include "../inc/jumptab.inc"

_GetRandom:
	    jsr GetRandom
	    ldx #0
	    rts
