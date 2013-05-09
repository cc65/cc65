;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: Scale the primary register
;

        .export         asreax1
        .importzp       sreg, tmp1

asreax1:
        stx     tmp1
        ldx     sreg+1
        cpx     #$80            ; Get bit 7 into carry
        ror     sreg+1
        ror     sreg
        ror     tmp1
        ror     a
        ldx     tmp1
        rts

