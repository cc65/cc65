;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: Scale the primary register by 4
;

        .export         shrax2
        .importzp       tmp1

shrax2: stx     tmp1
        lsr     tmp1
        ror     a
        lsr     tmp1
        ror     a
        ldx     tmp1
        rts


