;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

            .importzp ptr4
            .export BitmapRegs

            .include "geossym.inc"

BitmapRegs:                     ; a/x is a struct iconpic*
        sta ptr4
        stx ptr4+1
        ldy #0
bmpLp:  lda (ptr4),y
        sta r0L,y
        iny
        cpy #6
        bne bmpLp
        rts
