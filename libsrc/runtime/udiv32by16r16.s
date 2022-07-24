;
; Ullrich von Bassewitz, 2009-11-04
;
; CC65 runtime: 32by16 => 16 unsigned division
;

        .export         udiv32by16r16, udiv32by16r16m

        .include        "zeropage.inc"


;---------------------------------------------------------------------------
; 32by16 division. Divide ptr1:ptr2 by ptr3. Result is in ptr1, remainder
; in sreg.
;
;   lhs         rhs           result      result also in    remainder
; -----------------------------------------------------------------------
;   ptr1:ptr2   ptr3          ax          ptr1              sreg
;


udiv32by16r16:
        sta     ptr3
        stx     ptr3+1
udiv32by16r16m:
        lda     #0
        sta     sreg+1
        ldy     #32

L0:     asl     ptr1
        rol     ptr1+1
        rol     ptr2
        rol     ptr2+1
        rol     a
        rol     sreg+1

        tax
        cmp     ptr3
        lda     sreg+1
        sbc     ptr3+1
        bcc     L1

        sta     sreg+1
        txa
        sbc     ptr3
        tax
        inc     ptr1

L1:     txa
        dey
        bne     L0
        sta     sreg
        lda     ptr1
        ldx     ptr1+1
        rts

