;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: Scale the primary register by 4
;

        .export         asrax2
        .importzp       tmp1

asrax2: stx     tmp1
        cpx     #$80            ; Put bit 7 into carry
        ror     tmp1
        ror     a
        cpx     #$80
        ror     tmp1
        ror     a
        ldx     tmp1
        rts

