;
; Ullrich von Bassewitz, 03.06.1999
;
; unsigned char cbm_k_readst (void);
;

        .include        "cbm.inc"

        .export         _cbm_k_readst


_cbm_k_readst:
        jsr     READST
        ldx     #0              ; Clear high byte
        rts
