;
; Piotr Fusik, 24.10.2003
; originally by Ullrich von Bassewitz
;
; CC65 runtime: Scale the primary register by 8
;

        .export         asrax3
        .importzp       tmp1

asrax3: stx     tmp1
        cpx     #$80            ; Put bit 7 into carry
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

