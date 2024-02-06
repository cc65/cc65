;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: Scale the primary register
;

        .export         shrax1
        .importzp       tmp1

shrax1: stx     tmp1
        lsr     tmp1
        ror     a
        ldx     tmp1
        rts

