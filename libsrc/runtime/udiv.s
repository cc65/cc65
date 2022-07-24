;
; Ullrich von Bassewitz, 07.08.1998
;
; CC65 runtime: division for unsigned ints
;
; Don't use tmp1 here, the signed division tunnels data with it!

        .export         tosudiva0, tosudivax, udiv16
        .import         popptr1
        .importzp       sreg, ptr1, ptr4


tosudiva0:
        ldx     #$00            ; Clear high byte
tosudivax:
        sta     ptr4
        stx     ptr4+1          ; Save right operand
        jsr     popptr1         ; Get left operand

; Do the division

        jsr     udiv16

; Result is in ptr1, remainder in sreg

        lda     ptr1
        ldx     ptr1+1
        rts

;---------------------------------------------------------------------------
; 16by16 division. Divide ptr1 by ptr4. Result is in ptr1, remainder in sreg
; (see mult-div.s from "The Fridge").
; This is also the entry point for the signed division

udiv16: lda     #0
        sta     sreg+1
        ldy     #16
        ldx     ptr4+1
        beq     udiv16by8a

L0:     asl     ptr1
        rol     ptr1+1
        rol     a
        rol     sreg+1

        tax
        cmp     ptr4
        lda     sreg+1
        sbc     ptr4+1
        bcc     L1

        sta     sreg+1
        txa
        sbc     ptr4
        tax
        inc     ptr1

L1:     txa
        dey
        bne     L0
        sta     sreg
        rts


;---------------------------------------------------------------------------
; 16by8 division

udiv16by8a:
@L0:    asl     ptr1
        rol     ptr1+1
        rol     a
        bcs     @L1

        cmp     ptr4
        bcc     @L2
@L1:    sbc     ptr4
        inc     ptr1

@L2:    dey
        bne     @L0
        sta     sreg
        rts

