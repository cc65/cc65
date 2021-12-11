;
; Ullrich von Bassewitz, 03.06.1999
;
; unsigned char cbm_k_acptr (void);
;

        .include        "cbm.inc"

        .export         _cbm_k_acptr


_cbm_k_acptr:
        jsr     ACPTR
        ldx     #0
        rts
