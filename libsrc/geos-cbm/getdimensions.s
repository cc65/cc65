;
; 2026 cc65 project (added for PRINTER driver support, see grc65)
;

; void __fastcall__ GetDimensions (char *width, char *height, char *mode);

            .export _GetDimensions
            .import popax
            .importzp ptr1, ptr2, ptr3, tmp1, tmp2

            .include "printdrv.inc"

_GetDimensions:
        sta ptr3                ; mode -- passed in A/X (fastcall, pointer)
        stx ptr3+1
        jsr popax               ; height
        sta ptr1
        stx ptr1+1
        jsr popax               ; width
        sta ptr2
        stx ptr2+1
        jsr GetDimensions       ; returns capability flag in A, width in X, height in Y
        pha                     ; save capability flag
        stx tmp1
        sty tmp2
        ldy #0
        lda tmp1
        sta (ptr2),y            ; *width = tmp1
        lda tmp2
        sta (ptr1),y            ; *height = tmp2
        pla
        sta (ptr3),y            ; *mode = capability flag
        rts
