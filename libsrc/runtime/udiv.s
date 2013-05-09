;
; Ullrich von Bassewitz, 07.08.1998
;
; CC65 runtime: division for unsigned ints
;

        .export         tosudiva0, tosudivax, udiv16
        .import         popsreg
        .importzp       sreg, ptr1, ptr4


tosudiva0:
        ldx     #$00            ; Clear high byte
tosudivax:
        sta     ptr4
        stx     ptr4+1          ; Save right operand
        jsr     popsreg         ; Get left operand

; Do the division

        jsr     udiv16

; Result is in sreg, remainder in ptr1

        lda     sreg
        ldx     sreg+1
        rts

;---------------------------------------------------------------------------
; 16by16 division. Divide sreg by ptr4. Result is in sreg, remainder in ptr1
; (see mult-div.s from "The Fridge").
; This is also the entry point for the signed division

udiv16: lda     #0
        sta     ptr1+1
        ldy     #16
        ldx     ptr4+1
        beq     udiv16by8a

L0:     asl     sreg
        rol     sreg+1
        rol     a
        rol     ptr1+1

        pha
        cmp     ptr4
        lda     ptr1+1
        sbc     ptr4+1
        bcc     L1

        sta     ptr1+1
        pla
        sbc     ptr4
        pha
        inc     sreg

L1:     pla
        dey
        bne     L0
        sta     ptr1
        rts


;---------------------------------------------------------------------------
; 16by8 division

udiv16by8a:
@L0:    asl     sreg
        rol     sreg+1
        rol     a
        bcs     @L1

        cmp     ptr4
        bcc     @L2
@L1:    sbc     ptr4
        inc     sreg

@L2:    dey
        bne     @L0
        sta     ptr1
        rts

