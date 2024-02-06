;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: Scale the primary register
;

        .export         asrax1
        .importzp       tmp1

asrax1: stx     tmp1
        cpx     #$80            ; Put bit 7 into carry
        ror     tmp1
        ror     a
        ldx     tmp1
        rts

