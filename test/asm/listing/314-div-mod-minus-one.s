; Dividing the most negative value by -1 used to crash ca65 (SIGFPE).
; The quotient wraps around, the remainder is 0.

        .byte   <((1 << 63) / -1)       ; 00
        .byte   <((1 << 63) .mod -1)    ; 00
        .byte   <(-5 / -1)              ; 05
        .byte   <(7 .mod -1)            ; 00

; The same with values that are only known later (other code path)
        .byte   <(num / den)                ; 00
        .byte   <(num .mod den)             ; 00

num = 1 << 63
den = -1
