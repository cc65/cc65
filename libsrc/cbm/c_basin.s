;
; Ullrich von Bassewitz, 03.06.1999
;
; unsigned char cbm_k_basin (void);
;

        .include        "cbm.inc"

        .export         _cbm_k_basin


_cbm_k_basin:
        jsr     BASIN
        ldx     #0              ; Clear high byte
        rts
