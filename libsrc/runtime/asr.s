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


      	.export		tosasrax
      	.import		popax
      	.importzp	tmp1

tosasrax:
        and     #$0F            ; Bring the shift count into a valid range
        sta     tmp1            ; Save it

        jsr     popax           ; Get the left hand operand

        ldy     tmp1            ; Get shift count
        beq     L9              ; Bail out if shift count zero

        cpy     #8              ; Shift count 8 or greater?
        bcc     L1              ; Jump if not

; Shift count is greater 8. The carry is set when we enter here.

        tya
        sbc     #8
        tay                     ; Adjust shift count
        txa
        ldx     #$00            ; Shift by 8 bits
        cmp     #$00            ; Test sign bit
        bpl     L1
        dex                     ; Make X the correct sign extended value

; Save the high byte so we can shift it

L1:     stx     tmp1            ; Save high byte
        jmp     L3

; Do the actual shift

L2:    	cpx     #$80            ; Copy bit 15 into the carry
        ror     tmp1
        ror     a
L3:     dey
       	bpl     L2

; Done with shift

        ldx    	tmp1
L9:     rts



