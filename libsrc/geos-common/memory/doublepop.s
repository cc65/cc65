;
; Maciej 'YTM/Alliance' Witkowiak
;
; 31.12.99

            .import popax
            .export DoublePop

            .include "geossym.inc"

DoublePop:
        sta r0L
        stx r0H
        jsr popax
        sta r1L
        stx r1H
        rts
