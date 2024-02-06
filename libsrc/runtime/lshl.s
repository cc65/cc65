;
; Ullrich von Bassewitz, 2004-06-30
;
; CC65 runtime: left shift support for long and unsigned long
;
; Note: The standard declares a shift count that is negative or >= the
; bitcount of the shifted type for undefined behaviour.
;
; Note^2: The compiler knowns about the register/zero page usage of this
; function, so you need to change the compiler source if you change it!
;


        .export         tosasleax, tosshleax
        .import         popeax
        .importzp       sreg, tmp1


tosshleax:
tosasleax:
        and     #$1F            ; Bring the shift count into a valid range
        sta     tmp1            ; Save it

        jsr     popeax          ; Get the left hand operand

        ldy     tmp1            ; Get shift count
        beq     L9              ; Bail out if shift count zero
        stx     tmp1            ; Save byte 1

; Do the actual shift. Faster solutions are possible but need a lot more code.

L2:     asl     a
        rol     tmp1
        rol     sreg
        rol     sreg+1
        dey
        bne     L2

; Shift done

        ldx     tmp1
L9:     rts


