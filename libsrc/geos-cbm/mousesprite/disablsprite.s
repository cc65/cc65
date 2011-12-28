
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 30.10.99

; void DisablSprite (char spritenum);

	    .export _DisablSprite

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"

_DisablSprite:
	    sta r3L
	    jmp DisablSprite