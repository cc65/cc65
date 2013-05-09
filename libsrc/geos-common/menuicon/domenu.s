;
; Maciej 'YTM/Alliance' Witkowiak
;
; 30.10.99

; void DoMenu         (struct menu *mymenu);

            .export _DoMenu

            .include "jumptab.inc"
            .include "geossym.inc"

_DoMenu:
        sta r0L
        stx r0H
        lda #0
        jmp DoMenu