;
; Ullrich von Bassewitz, 2009-08-17
;
; CC65 runtime: multiplication for ints. Short versions.
;

        .export         tosumula0, tosmula0
        .export         mul8x16, mul8x16a
        .import         popptr1
        .importzp       ptr1, ptr4


;---------------------------------------------------------------------------
; 8x16 routine with external entry points used by the 16x16 routine in mul.s

tosmula0:
tosumula0:
        sta     ptr4
mul8x16:jsr     popptr1         ; Get left operand (Y=0 by popptr1)

        tya                     ; Clear byte 1
        ldy     #8              ; Number of bits
        ldx     ptr1+1          ; check if lhs is 8 bit only
        beq     mul8x8          ; Do 8x8 multiplication if high byte zero
mul8x16a:
        sta     ptr4+1          ; Clear byte 2

        lsr     ptr4            ; Get first bit into carry
@L0:    bcc     @L1

        clc
        adc     ptr1
        tax
        lda     ptr1+1          ; hi byte of left op
        adc     ptr4+1
        sta     ptr4+1
        txa

@L1:    ror     ptr4+1
        ror     a
        ror     ptr4
        dey
        bne     @L0
        tax
        lda     ptr4            ; Load the result
        rts

;---------------------------------------------------------------------------
; 8x8 multiplication routine

mul8x8:
        lsr     ptr4            ; Get first bit into carry
@L0:    bcc     @L1
        clc
        adc     ptr1
@L1:    ror
        ror     ptr4
        dey
        bne     @L0
        tax
        lda     ptr4            ; Load the result
        rts                     ; Done

