;
; Ullrich von Bassewitz, 1998-08-05, 2004-06-25
;
; CC65 runtime: left shift support for ints and unsigneds
;
; Note: The standard declares a shift count that is negative or >= the
; bitcount of the shifted type for undefined behaviour.
;
; Note^2: The compiler knowns about the register/zero page usage of this
; function, so you need to change the compiler source if you change it!
;


        .export          tosaslax, tosshlax, aslaxy, shlaxy
        .import          popax
        .importzp        tmp1

        .macpack        cpu

tosshlax:
tosaslax:
        sta     tmp1            ; Save shift count
        jsr     popax           ; Get the left hand operand
        ldy     tmp1            ; Get shift count

; Run into shlaxy

shlaxy:
aslaxy:
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
        pla                     ; Get low byte

L1:     asl     a
        dey
        bne     L1
        tax
        tya                     ; A = 0
        rts

; Shift count is zero

L2:     pla
        rts

; Shift count is exactly 8

.if (.cpu .bitand CPU_ISET_65SC02)
L3:     plx                     ; Low byte from stack into X
        rts                     ; A is already zero
.else
L3:     pla                     ; Low byte from stack ...
        tax                     ; ... into X
        lda     #$00            ; Clear low byte
        rts
.endif

; Shift count is less than 8

L4:     adc     #8              ; Correct counter
        tay                     ; Shift count into Y
        pla                     ; Restore low byte
        stx     tmp1            ; Save high byte of lhs
L5:     asl     a
        rol     tmp1
        dey
        bne     L5

; Done with shift

        ldx     tmp1
L9:     rts

