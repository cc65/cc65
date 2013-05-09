;
; Maciej 'YTM/Alliance' Witkowiak
;
; 25.12.99

; void GraphicsString         (char *myString);

            .export _GraphicsString

            .include "jumptab.inc"
            .include "geossym.inc"

_GraphicsString:
        sta r0L
        stx r0H
        jmp GraphicsString
