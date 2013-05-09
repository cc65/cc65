;
; Ullrich von Bassewitz, 03.06.1999
;
; unsigned char cbm_k_acptr (void);
;

        .export         _cbm_k_acptr
        .import         ACPTR


_cbm_k_acptr:
        jsr     ACPTR
        ldx     #0
        rts
