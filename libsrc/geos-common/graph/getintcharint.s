;
; Maciej 'YTM/Alliance' Witkowiak
;
; 11.03.2000

            .import popa, popax
            .export getintcharint

            .include "geossym.inc"

getintcharint:
        sta r11L
        stx r11H
        jsr popa
        sta r1H
        jsr popax
        sta r0L
        stx r0H
        rts

