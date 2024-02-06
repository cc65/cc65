;
; Piotr Fusik, 24.10.2003
; originally by Ullrich von Bassewitz
;
; CC65 runtime: Scale the primary register by 16
;

        .export         asrax4
        .importzp       tmp1

asrax4: stx     tmp1
        cpx     #$80            ; Put bit 7 into carry
        ror     tmp1
        ror     a
        cpx     #$80
        ror     tmp1
        ror     a
        cpx     #$80
        ror     tmp1
        ror     a
        cpx     #$80
        ror     tmp1
        ror     a
        ldx     tmp1
        rts

