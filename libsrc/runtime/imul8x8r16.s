;
; 2010-11-02, Ullrich von Bassewitz
; 2014-09-10, Greg King
; 2019-11-05, Piotr Fusik
;
; CC65 runtime: 8x8 => 16 signed multiplication
;

        .export         imul8x8r16, imul8x8r16m
        .importzp       ptr1, ptr3, tmp1

        .macpack        generic

;---------------------------------------------------------------------------
; 8x8 => 16 signed multiplication routine.
;
;   multiplicand  multiplier   product
;   LHS             RHS        result
; -------------------------------------------------------------
;   .A (ptr3-low)   ptr1-low    .XA
;

imul8x8r16:
        sta     ptr3

imul8x8r16m:
; Extend sign of Left-Hand Side
        lda     #$7f
        cmp     ptr3
        adc     #$80
        sta     ptr3+1

; Clear .XY accumulator
        ldy     #<$0000
        ldx     #>$0000

; Check the multiplier sign.
        lda     ptr1
        bpl     PosStart

; The multiplier is negative.
; Therefore, make it positive; and, subtract when multiplying.
NegMult:
        eor     #%11111111
        sta     ptr1
        inc     ptr1
        bnz     NegStart        ; Branch always

NegAdd:
        tya                     ; Subtract current multiplicand
;       sec
        sbc     ptr3
        tay
        txa
        sbc     ptr3+1
        tax

NegShift:
        asl     ptr3
        rol     ptr3+1
NegStart:
        lsr     ptr1            ; Get next bit of Right-Hand Side into carry
        bcs     NegAdd
        bnz     NegShift        ; Loop if more one-bits in multiplier

        tya                     ; Put result into cc65's accumulator
        rts

; The multiplier is positive.

PosAdd:
        tya                     ; Add current multiplicand
        add     ptr3
        tay
        txa
        adc     ptr3+1
        tax

PosShift:
        asl     ptr3
        rol     ptr3+1
PosStart:
        lsr     ptr1            ; Get next bit of Right-Hand Side into carry
        bcs     PosAdd
        bnz     PosShift        ; Loop if more one-bits in multiplier

        tya                     ; Put result into cc65's accumulator
        rts
