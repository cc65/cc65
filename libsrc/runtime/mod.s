;
; Christian Krueger, 24-May-2018
;
; CC65 runtime: modulo operation for signed ints
;

; When negating values, we will ignore the possibility here, that one of the
; values is $8000, in which case the negate will fail.

        .export         tosmoda0, tosmodax
        .import         absvaludiv16, negax
        .importzp       sp, sreg, tmp1

tosmoda0:
        ldx     #0
tosmodax:

; Prepare adjustment of the sign of the result. The sign of the result of the
; modulo operation is the same as that of the left operand.

        pha                         
        ldy     #1                  ; Prepare lhs operant hi-byte fetch
        lda     (sp),y             
        sta     tmp1                ; Save post negation indicator to tmp1
        pla                         ; Back to entry accu
        jsr     absvaludiv16
        ldx     sreg+1              ; Remainder to return registers
        lda     sreg
        ldy     tmp1                ; Fetch indicator
        bmi     negate
        rts
negate: jmp     negax
