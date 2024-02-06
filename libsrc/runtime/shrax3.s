;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: Scale the primary register by 8
;

        .export         shrax3
        .importzp       tmp1

shrax3: stx     tmp1
        lsr     tmp1
        ror     a
        lsr     tmp1
        ror     a
        lsr     tmp1
        ror     a
        ldx     tmp1
        rts


