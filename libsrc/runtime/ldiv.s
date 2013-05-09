;
; Ullrich von Bassewitz, 17.08.1998
;
; CC65 runtime: division for signed long ints
;

; When negating values, we will ignore the possibility here, that one of the
; values if $80000000, in which case the negate will fail.

        .export         tosdiv0ax, tosdiveax
        .import         poplsargs, udiv32, negeax
        .importzp       sreg, ptr1, tmp1, tmp2

tosdiv0ax:
        ldy     #$00
        sty     sreg
        sty     sreg+1

tosdiveax:
        jsr     poplsargs       ; Get arguments from stack, adjust sign
        jsr     udiv32          ; Do the division, result is in (ptr1:sreg)
        ldx     ptr1+1          ; Load byte 1 of result

; Adjust the sign of the result

        lda     tmp1            ; Get sign of left operand
        eor     tmp2            ; Calculate sign of result
        bpl     Pos             ; Jump if result positive

; Result is negative

        lda     ptr1            ; Load byte 0
        jmp     negeax          ; Negate value

; Result is positive

Pos:    lda     ptr1
        rts


