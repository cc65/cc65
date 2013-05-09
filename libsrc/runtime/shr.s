;
; Ullrich von Bassewitz, 2004-06-30
;
; CC65 runtime: right shift support for unsigneds
;
; Note: The standard declares a shift count that is negative or >= the
; bitcount of the shifted type for undefined behaviour.
;
; Note^2: The compiler knowns about the register/zero page usage of this
; function, so you need to change the compiler source if you change it!
;


        .export         tosshrax, shraxy
        .import         popax
        .importzp       tmp1

tosshrax:
        sta     tmp1            ; Save shift count
        jsr     popax           ; Get the left hand operand
        ldy     tmp1            ; Get shift count

; Run into shraxy

shraxy:
        pha
        tya
        and     #$0F
        beq     L2              ; Nothing to shift
        sec
        sbc     #8              ; Shift count 8 or greater?
        beq     L3              ; Jump if exactly 8
        bcc     L4              ; Jump if less than 8

; Shift count is greater than 8.

        tay                     ; Shift count into Y
        pla                     ; Discard low byte
        txa                     ; Get high byte

L1:     lsr     a
        dey
        bne     L1
        ldx     #$00            ; High byte is zero
        rts

; Shift count is zero

L2:     pla
        rts

; Shift count is exactly 8

L3:     pla                     ; Drop low byte from stack ...
        txa                     ; Move high byte to low
        ldx     #$00            ; Clear high byte
        rts

; Shift count is less than 8

L4:     adc     #8              ; Correct counter
        tay                     ; Shift count into Y
        pla                     ; Restore low byte
        stx     tmp1            ; Save high byte of lhs
L5:     lsr     tmp1
        ror     a
        dey
        bne     L5

; Done with shift

        ldx     tmp1
        rts


