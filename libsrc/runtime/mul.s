;
; Ullrich von Bassewitz, 2009-08-17
;
; CC65 runtime: multiplication for ints
;

        .export         tosumulax, tosmulax
        .import         mul8x16, mul8x16a       ; in mul8.s
        .import         popptr1
        .importzp       tmp1, ptr1, ptr4


;---------------------------------------------------------------------------
; 16x16 multiplication routine

tosmulax:
tosumulax:
        sta     ptr4
        txa                     ; High byte zero
        beq     @L3             ; Do 8x16 multiplication if high byte zero
        stx     ptr4+1          ; Save right operand
        jsr     popptr1         ; Get left operand (Y=0, X untouched by popptr1)

; Do ptr4:ptr4+1 * ptr1:ptr1+1 --> AX

        tya                     ; A = 0        
        ldy     ptr1+1          ; check if lhs is 8 bit only
        beq     @L4             ; -> we can do 8x16 after swap
        sta     tmp1
        ldy     #16             ; Number of bits

        lsr     ptr4+1
        ror     ptr4            ; Get first bit into carry
@L0:    bcc     @L1

        clc
        adc     ptr1
        tax
        lda     ptr1+1          ; Hi byte of left op         
        adc     tmp1
        sta     tmp1
        txa

@L1:    ror     tmp1
        ror     a
        ror     ptr4+1
        ror     ptr4
        dey
        bne     @L0

        lda     ptr4            ; Load the result
        ldx     ptr4+1
        rts                     ; Done

; High byte of rhs is zero, jump to the 8x16 routine instead

@L3:    jmp     mul8x16

; If the high byte of lhs is zero, swap the operands in ptr1/4 and
; use the 8x16 routine. On entry, A and Y are zero and X has the value
; of ptr4+1

@L4:    stx     ptr1+1          ; Store hi-byte from ptr4
        ldy     ptr1            ; Save right operand (8 bit)
        ldx     ptr4            ; Copy left 16 bit operand to right
        stx     ptr1
        sty     ptr4            ; Copy low 8 bit of right op to left
        ldy     #8
        jmp     mul8x16a        ; There, ptr4+1 will be also cleared

