;
; Ullrich von Bassewitz, 07.08.1998
;
; CC65 runtime: modulo operation for signed ints
;

; When negating values, we will ignore the possibility here, that one of the
; values if $8000, in which case the negate will fail.

        .export         tosmoda0, tosmodax
        .import         popsargs, udiv16, negax
        .importzp       ptr1, tmp1

tosmoda0:
        ldx     #0
tosmodax:
        jsr     popsargs        ; Get arguments from stack, adjust sign
        jsr     udiv16          ; Do the division
        lda     ptr1            ; Load low byte of result
        ldx     ptr1+1          ; Load high byte of result

; Adjust the sign of the result. tmp1 contains the high byte of the left
; operand, tmp2 contains the high byte of the right operand. The sign of
; the result of the modulo operation is the same as that of the left
; operand

        bit     tmp1
        bpl     Pos             ; Jump if sign of result positive

; Result is negative

        jmp     negax           ; Adjust the sign

; Result is positive

Pos:    rts

