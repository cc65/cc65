;
; Ullrich von Bassewitz, 2004-06-30
;
; CC65 runtime: right shift support for ints
;
; Note: The standard declares a shift count that is negative or >= the
; bitcount of the shifted type for undefined behaviour.
;
; Note^2: The compiler knowns about the register/zero page usage of this
; function, so you need to change the compiler source if you change it!
;


        .export         tosasrax, asraxy
        .import         popax
        .importzp       tmp1

tosasrax:
        sta     tmp1            ; Save shift count
        jsr     popax           ; Get the left hand operand
        ldy     tmp1            ; Get shift count

; Run into asraxy

asraxy:
        pha
        tya
        and     #$0F
        beq     L2              ; Nothing to shift
        sec
        sbc     #8              ; Shift count 8 or greater?
        beq     L3              ; Jump if exactly 8
        bcc     L6              ; Jump if less than 8

; Shift count is greater than 8.

        tay                     ; Shift count into Y
        pla                     ; Discard low byte
        txa                     ; Get high byte

L1:     cmp     #$80            ; Sign bit into carry
        ror     a               ; Carry into A
        dey
        bne     L1
        beq     L4              ; Sign extend and return

; Shift count is zero

L2:     pla
        rts

; Shift count is exactly 8

L3:     pla                     ; Drop low byte from stack ...
        txa                     ; Move high byte to low
L4:     ldx     #$00            ; Clear high byte
        cmp     #$80            ; Check sign bit
        bcc     L5
        dex
L5:     rts

; Shift count is less than 8

L6:     adc     #8              ; Correct counter
        tay                     ; Shift count into Y
        pla                     ; Restore low byte
        stx     tmp1            ; Save high byte of lhs
L7:     cpx     #$80            ; Sign bit into carry
        ror     tmp1
        ror     a
        dey
        bne     L7

; Done with shift

        ldx     tmp1
        rts

