;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: Scale the 32 bit primary register by 2
;

        .export         shreax1
        .importzp       sreg, tmp1

shreax1:
        stx     tmp1
        lsr     sreg+1
        ror     sreg
        ror     tmp1
        ror     a
        ldx     tmp1
        rts

