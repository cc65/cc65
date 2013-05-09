;
; Maciej 'YTM/Alliance' Witkowiak
;
; 29.10.99

; void InvertLine     (char y, int xstart, int xend);

            .import HLineRegs
            .export _InvertLine

            .include "jumptab.inc"
            
_InvertLine:
        jsr HLineRegs
        jmp InvertLine
