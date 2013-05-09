;
; Ullrich von Bassewitz, 2009-08-17
;
; CC65 runtime: multiplication for ints
;

        .export         tosumulax, tosmulax
        .import         mul8x16, mul8x16a       ; in mul8.s
        .import         popsreg
        .importzp       sreg, tmp1, ptr4


;---------------------------------------------------------------------------
; 16x16 multiplication routine

tosmulax:
tosumulax:
        sta     ptr4
        txa                     ; High byte zero
        beq     @L3             ; Do 8x16 multiplication if high byte zero
        stx     ptr4+1          ; Save right operand
        jsr     popsreg         ; Get left operand

; Do ptr4:ptr4+1 * sreg:sreg+1 --> AX

        lda     #0
        ldx     sreg+1          ; Get high byte into register for speed
        beq     @L4             ; -> we can do 8x16 after swap
        sta     tmp1
        ldy     #16             ; Number of bits

        lsr     ptr4+1
        ror     ptr4            ; Get first bit into carry
@L0:    bcc     @L1

        clc
        adc     sreg
        pha
        txa                     ; hi byte of left op
        adc     tmp1
        sta     tmp1
        pla

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

; If the high byte of rhs is zero, swap the operands and use the 8x16
; routine. On entry, A and X are zero

@L4:    ldy     sreg            ; Save right operand (8 bit)
        ldx     ptr4            ; Copy left 16 bit operand to right
        stx     sreg
        ldx     ptr4+1          ; Don't store, this is done later
        sty     ptr4            ; Copy low 8 bit of right op to left
        ldy     #8
        jmp     mul8x16a

