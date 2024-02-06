;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: Scale the 32 bit primary register by 4
;

        .export         asreax2
        .importzp       sreg, tmp1

asreax2:
        stx     tmp1
        ldx     sreg+1
        cpx     #$80            ; Get bit 7 into carry
        ror     sreg+1
        ror     sreg
        ror     tmp1
        ror     a
        cpx     #$80            ; Get bit 7 into carry
        ror     sreg+1
        ror     sreg
        ror     tmp1
        ror     a
        ldx     tmp1
        rts

