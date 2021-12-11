;
; 1999-06-03, Ullrich von Bassewitz
; 2021-01-12, Greg King
;
; unsigned char cbm_k_readst (void);
;

        .include        "cbm.inc"

        .export         _cbm_k_readst


_cbm_k_readst:
        ldx     #>$0000
        jmp     READST
