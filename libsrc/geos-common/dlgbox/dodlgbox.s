;
; Maciej 'YTM/Elysium' Witkowiak
;
; 25.12.1999, 2.1.2003

; char DoDlgBox         (char *myParamString);

            .export _DoDlgBox

            .include "jumptab.inc"
            .include "geossym.inc"

_DoDlgBox:
        sta r0L
        stx r0H
        jsr DoDlgBox
        ldx #0
        lda r0L
        rts
