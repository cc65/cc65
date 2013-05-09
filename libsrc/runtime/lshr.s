;
; Ullrich von Bassewitz, 2004-06-30
;
; CC65 runtime: right shift support for unsigned longs
;
; Note: The standard declares a shift count that is negative or >= the
; bitcount of the shifted type for undefined behaviour.
;
; Note^2: The compiler knowns about the register/zero page usage of this
; function, so you need to change the compiler source if you change it!
;


        .export         tosshreax
        .import         popeax
        .importzp       sreg, tmp1


tosshreax:
        and     #$1F            ; Bring the shift count into a valid range
        sta     tmp1            ; Save it

        jsr     popeax          ; Get the left hand operand

        ldy     tmp1            ; Get shift count
        beq     L9              ; Bail out if shift count zero
        stx     tmp1            ; Save byte 1

; Do the actual shift. Faster solutions are possible but need a lot more code.

L2:     lsr     sreg+1
        ror     sreg
        ror     tmp1
        ror     a
        dey
        bne     L2

; Shift done

        ldx     tmp1
L9:     rts







