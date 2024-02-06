;
; Ullrich von Bassewitz, 25.07.2001
;
; CC65 runtime: Scale the 32 bit primary register by 16
;

        .export         shreax4
        .importzp       sreg, tmp1

shreax4:
        stx     tmp1
        ldx     #4
@L1:    lsr     sreg+1
        ror     sreg
        ror     tmp1
        ror     a
        dex
        bne     @L1
        ldx     tmp1
        rts

