;
; Ullrich von Bassewitz, 25.07.2001
;
; CC65 runtime: Scale the 32 bit primary register by 16
;

        .export         asreax4
        .importzp       sreg, tmp1

asreax4:
        stx     tmp1
        ldx     sreg+1
        ldy     #4
@L1:    cpx     #$80            ; Get bit 7 into carry
        ror     sreg+1
        ror     sreg
        ror     tmp1
        ror     a
        dey
        bne     @L1
        ldx     tmp1
        rts

