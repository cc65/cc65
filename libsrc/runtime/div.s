;
; Christian Krueger, 24-May-2018
;
; CC65 runtime: division for signed ints
;

; When negating values, we will ignore the possibility here, that one of the
; values is $8000, in which case the negate will fail.

        .export         tosdiva0, tosdivax
        .import         absvaludiv16, negax
        .importzp       sp, ptr1, tmp1

tosdiva0:
        ldx     #0
tosdivax:
        pha                         ; Check if high-bytes indicate
        txa                         ; different sign, so that we have to
        ldy     #1                  ; negate the result after the operation.
        eor     (sp),y              ; Eor with lhs high byte
        sta     tmp1                ; Save post negation indicator to tmp1
        pla                         ; Back to entry accu
        jsr     absvaludiv16
        ldx     ptr1+1
        lda     ptr1
        ldy     tmp1                ; Fetch indicator
        bmi     negate
        rts
negate: jmp     negax
