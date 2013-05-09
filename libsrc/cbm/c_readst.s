;
; Ullrich von Bassewitz, 03.06.1999
;
; unsigned char cbm_k_readst (void);
;

        .export         _cbm_k_readst
        .import         READST


_cbm_k_readst:
        jsr     READST
        ldx     #0              ; Clear high byte
        rts
