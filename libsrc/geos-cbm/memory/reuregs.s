;
; Maciej 'YTM/Alliance' Witkowiak
;
; 31.12.99

            .import popax, popa
            .import DoublePop
            .export REURegs

            .include "geossym.inc"

REURegs:
        jsr DoublePop
        jsr popax
        sta r2L
        stx r2H
        jsr popa
        sta r3L
        rts

