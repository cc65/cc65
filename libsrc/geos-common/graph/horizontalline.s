;
; Maciej 'YTM/Alliance' Witkowiak
;
; 29.10.99

; void HorizontalLine (char pattern, char y, int xstart, int xend);

            .import popa
            .import HLineRegs
            .export _HorizontalLine

            .include "jumptab.inc"

_HorizontalLine:
        jsr HLineRegs
        jsr popa
        jmp HorizontalLine
