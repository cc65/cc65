
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 27.10.2001

; unsigned char kbhit (void);

	    .export _kbhit
	    .import return0, return1

	    .include "../inc/geossym.inc"

_kbhit:
	    lda pressFlag
	    bmi L1
	    jmp return0
L1:	    jmp return1
