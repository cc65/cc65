;
; Ullrich von Bassewitz, 25.07.2001
;
; CC65 runtime: Scale the primary register by 16
;

        .export         shrax4
        .importzp       tmp1

shrax4: stx     tmp1
        lsr     tmp1
        ror     a
        lsr     tmp1
        ror     a
        lsr     tmp1
        ror     a
        lsr     tmp1
        ror     a
        ldx     tmp1
        rts


