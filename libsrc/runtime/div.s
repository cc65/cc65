;
; Ullrich von Bassewitz, 07.08.1998
;
; CC65 runtime: division for signed ints
;

; When negating values, we will ignore the possibility here, that one of the
; values is $8000, in which case the negate will fail.

        .export         tosdiva0, tosdivax
        .import         popsargsudiv16, negax
        .importzp       ptr1, tmp1, tmp2

tosdiva0:
        ldx     #0
tosdivax:
        jsr     popsargsudiv16  ; Get arguments from stack, adjust sign
                                ; and do the division
        ldx     ptr1+1          ; Load high byte of result

; Adjust the sign of the result. tmp1 contains the high byte of the left
; operand, tmp2 contains the high byte of the right operand.

        lda     tmp1
        eor     tmp2
        bpl     Pos             ; Jump if sign of result positive

; Result is negative

        lda     ptr1            ; Load low byte of result
        jmp     negax           ; Adjust the sign

; Result is positive

Pos:    lda     ptr1            ; Load low byte of result
        rts

