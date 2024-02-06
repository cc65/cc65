;
; Maciej 'YTM/Alliance' Witkowiak
;
; 29.10.99

            .importzp ptr4
            .export PointRegs

            .include "geossym.inc"

PointRegs:                      ; a/x is a struct pixel*
        sta ptr4
        stx ptr4+1
        ldy #0
        lda (ptr4),y
        sta r3L
        iny
        lda (ptr4),y
        sta r3H
        iny
        lda (ptr4),y
        sta r11L
        rts
