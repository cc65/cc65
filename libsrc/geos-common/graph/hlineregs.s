;
; Maciej 'YTM/Alliance' Witkowiak
;
; 29.10.99


            .import popax, popa
            .export HLineRegs

            .include "geossym.inc"

HLineRegs:
        stx r4H
        sta r4L
        jsr popax
        stx r3H
        sta r3L
        jsr popa
        sta r11L
        rts
