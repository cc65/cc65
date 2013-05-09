;
; Maciej 'YTM/Alliance' Witkowiak
;
; 30.10.99

; void PosSprite (char spritenum, struct pixel *position );

            .importzp ptr4
            .import popa
            .export _PosSprite

            .include "jumptab.inc"
            .include "geossym.inc"

_PosSprite:
        sta ptr4
        stx ptr4+1
        ldy #0
        lda (ptr4),y
        sta r4L
        iny
        lda (ptr4),y
        sta r4H
        iny
        lda (ptr4),y
        sta r5L
        jsr popa
        sta r3L
        jmp PosSprite