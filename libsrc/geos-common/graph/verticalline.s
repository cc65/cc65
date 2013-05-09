;
; Maciej 'YTM/Alliance' Witkowiak
;
; 29.10.99

; void VerticalLine   (char pattern, char ystart, char yend, int x);

            .import popa 
            .export _VerticalLine

            .include "jumptab.inc"
            .include "geossym.inc"
            
_VerticalLine:
        stx r4H
        sta r4L
        jsr popa
        sta r3H
        jsr popa
        sta r3L
        jsr popa
        jmp VerticalLine
