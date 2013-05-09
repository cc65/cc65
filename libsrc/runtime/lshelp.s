;
; Ullrich von Bassewitz, 13.08.1998
;
; CC65 runtime: helper stuff for mod/div/mul with long signed ints
;

; When negating values, we will ignore the possibility here, that one of the
; values if $80000000, in which case the negate will fail.

        .export         poplsargs
        .import         getlop
        .importzp       sreg, tmp1, tmp2, ptr1, ptr3, ptr4

poplsargs:
        jsr     getlop          ; Get the operands

; Remember the signs of the operands (that is, the high bytes) in tmp1 and
; tmp2. Make both operands positive.

        lda     sreg+1          ; Is the left operand negative?
        sta     tmp1            ; Remember the sign for later
        bpl     L1              ; Jump if not

        clc                     ; Make it positive
        lda     ptr1
        eor     #$FF
        adc     #$01
        sta     ptr1
        lda     ptr1+1
        eor     #$FF
        adc     #$00
        sta     ptr1+1
        lda     sreg
        eor     #$FF
        adc     #$00
        sta     sreg
        lda     sreg+1
        eor     #$FF
        adc     #$00
        sta     sreg+1

L1:     lda     ptr4+1          ; Is the right operand nagative?
        sta     tmp2            ; Remember the sign for later
        bpl     L2              ; Jump if not

        clc                     ; Make it positive
        lda     ptr3
        eor     #$FF
        adc     #$01
        sta     ptr3
        lda     ptr3+1
        eor     #$FF
        adc     #$00
        sta     ptr3+1
        lda     ptr4
        eor     #$FF
        adc     #$00
        sta     ptr4
        lda     ptr4+1
        eor     #$FF
        adc     #$00
        sta     ptr4+1

L2:     rts

