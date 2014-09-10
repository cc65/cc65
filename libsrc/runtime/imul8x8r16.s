;
; 2010-11-02, Ullrich von Bassewitz
; 2014-09-10, Greg King
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
        ldx     #>$0000
        bit     ptr3
        bpl     @L7
        dex
@L7:    stx     ptr3+1          ; Extend sign of Left-Hand Side
        ldy     #<$0000         ; Clear .XY accumulator
        ldx     #>$0000
        lda     ptr1
        bmi     NegMult
        bpl     @L2             ; Branch always

@L0:    tya                     ; Add current multiplicand
        add     ptr3
        tay
        txa
        adc     ptr3+1
        tax

@L1:    asl     ptr3
        rol     ptr3+1
@L2:    lsr     ptr1            ; Get next bit of Right-Hand Side into carry
        bcs     @L0
        bnz     @L1             ; Loop if more one-bits in multiplier

        tya                     ; Put result into cc65's accumulator
        rts

; The multiplier is negative.
; Therefore, make it positive; and, subtract when multiplying.
NegMult:
        eor     #%11111111
        sta     ptr1
        inc     ptr1
        bnz     @L2             ; Branch always

@L0:    tya                     ; Subtract current multiplicand
        sub     ptr3
        tay
        txa
        sbc     ptr3+1
        tax

@L1:    asl     ptr3
        rol     ptr3+1
@L2:    lsr     ptr1            ; Get next bit of Right-Hand Side into carry
        bcs     @L0
        bnz     @L1             ; Loop if more one-bits in multiplier

        tya                     ; Put result into cc65's accumulator
        rts
