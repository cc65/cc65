;
; Maciej 'YTM/Alliance' Witkowiak
;
; 30.10.99

; void EnablSprite (char spritenum);

            .export _EnablSprite

            .include "jumptab.inc"
            .include "geossym.inc"

_EnablSprite:
        sta r3L
        jmp EnablSprite