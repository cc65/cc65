;
; Maciej 'YTM/Alliance' Witkowiak
;
; 29.10.99
; 11.03.2000

; void InitDrawWindow (struct window *myWindow);

            .importzp ptr4
            .export _InitDrawWindow

            .include "geossym.inc"

_InitDrawWindow:                ; a/x is a struct window*
        sta ptr4
        stx ptr4+1
        ldy #0
copyWin:lda (ptr4),y
        sta r2L,y
        iny
        cpy #6
        bne copyWin
        rts
