;
; Maciej 'YTM/Elysium' Witkowiak
;
; 22.12.99, 29.07.2000

            .import popax
            .importzp ptr3, ptr4
            .export DoubleSPop
            .export SetPtrXY

            .include "geossym.inc"

DoubleSPop:
        sta ptr4
        stx ptr4+1
        jsr popax
        sta ptr3
        stx ptr3+1
;       rts
;
; SetPtrXY can be sometimes executed twice, but even this way it is few cycles
; faster...

SetPtrXY:
        ldx #ptr4
        ldy #ptr3
        rts
