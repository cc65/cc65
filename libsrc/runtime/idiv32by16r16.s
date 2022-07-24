;
; Ullrich von Bassewitz, 2009-11-04
;
; CC65 runtime: 32by16 => 16 signed division
;

        .export         idiv32by16r16
        .import         negax, udiv32by16r16m

        .include        "zeropage.inc"


;---------------------------------------------------------------------------
; 32by16 division. Divide ptr1:ptr2 by ptr3. Result is in ptr1, remainder
; in sreg.
;
;   lhs         rhs           result      result also in    remainder
; -----------------------------------------------------------------------
;   ptr1:ptr2   ptr3          ax          ptr1              sreg
;


idiv32by16r16:
        stx     tmp1
        cpx     #0
        bpl     @L1
        jsr     negax
@L1:    sta     ptr3
        stx     ptr3+1

        lda     ptr2+1
        cmp     #$80
        eor     tmp1
        sta     tmp1
        bcc     @L3

; Negate the value in ptr1:ptr2

        ldx     #0
        ldy     #4
;       sec
@L2:    lda     #$00
        sbc     ptr1,x
        sta     ptr1,x
        inx
        dey
        bne     @L2

; Call the unsigned division routine

@L3:    jsr     udiv32by16r16m

; Check the sign of the result

        bit     tmp1
        bmi     @L4
        rts

; Negate the result. We do this here only for the result, not for the
; remainder!

@L4:    jmp     negax

