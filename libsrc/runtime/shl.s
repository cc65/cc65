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


	.export		tosaslax, tosshlax
	.import		popax
	.importzp	tmp1

tosshlax:
tosaslax:
        and     #$0F            ; Bring the shift count into a valid range
        sta     tmp1            ; Save it

        jsr     popax           ; Get the left hand operand

        ldy     tmp1            ; Get shift count
        beq     L9              ; Bail out if shift count zero

        cpy     #8              ; Shift count 8 or greater?
        bcc     L3              ; Jump if not

; Shift count is greater 7. The carry is set when we enter here.

        tax
        tya
        sbc     #8
        tay
        txa
        jmp     L2
L1:     asl     a
L2:     dey
        bpl     L1
        tax
        lda     #$00
        rts

; Shift count is less than 8.

L3:     stx     tmp1            ; Save high byte of lhs
L4:    	asl	a
  	rol	tmp1
        dey
       	bne     L4

; Done with shift

        ldx	tmp1
L9:     rts

