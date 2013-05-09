;
; Ullrich von Bassewitz, 07.08.1998
;
; CC65 runtime: division for signed ints
;

; When negating values, we will ignore the possibility here, that one of the
; values if $8000, in which case the negate will fail.

        .export         tosdiva0, tosdivax
        .import         popsargs, udiv16, negax
        .importzp       sreg, tmp1, tmp2

tosdiva0:
        ldx     #0
tosdivax:
        jsr     popsargs        ; Get arguments from stack, adjust sign
        jsr     udiv16          ; Do the division
        ldx     sreg+1          ; Load high byte of result

; Adjust the sign of the result. tmp1 contains the high byte of the left
; operand, tmp2 contains the high byte of the right operand.

        lda     tmp1
        eor     tmp2
        bpl     Pos             ; Jump if sign of result positive

; Result is negative

        lda     sreg            ; Load low byte of result
        jmp     negax           ; Adjust the sign

; Result is positive

Pos:    lda     sreg
        rts

