;
; 2026 cc65 project
;

; char __fastcall__ GetRealSize (char character, char mode, char *height, char *baseline);

            .export _GetRealSize
            .import popa, popax
            .importzp ptr1, ptr2, tmp1

            .include "jumptab.inc"

_GetRealSize:
        sta ptr2                ; baseline
        stx ptr2+1
        jsr popax               ; height
        sta ptr1
        stx ptr1+1
        jsr popa                ; mode
        sta tmp1
        jsr popa                ; character
        ldx tmp1
        jsr GetRealSize         ; returns width in Y, height in X, baseline in A
        sty tmp1
        ldy #0
        sta (ptr2),y
        txa
        sta (ptr1),y
        lda tmp1
        ldx #0
        rts
