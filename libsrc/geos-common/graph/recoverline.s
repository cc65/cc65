;
; Maciej 'YTM/Alliance' Witkowiak
;
; 29.10.99

; void RecoverLine    (char y, int xstart, int xend);

            .import HLineRegs
            .export _RecoverLine
            
            .include "jumptab.inc"

_RecoverLine:
        jsr HLineRegs
        jmp RecoverLine

