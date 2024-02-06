;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; void PromptOn (struct pixel *);

            .importzp ptr4
            .export _PromptOn

            .include "jumptab.inc"
            .include "geossym.inc"

_PromptOn:
        sta ptr4
        stx ptr4+1
        ldy #0
promptLp:
        lda (ptr4),y
        sta stringX,y
        iny
        cpy #3
        bne promptLp
        jmp PromptOn
