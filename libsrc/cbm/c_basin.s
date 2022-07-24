;
; Ullrich von Bassewitz, 03.06.1999
;
; unsigned char cbm_k_basin (void);
; unsigned char cbm_k_chrin (void);
;

        .include        "cbm.inc"

        .export         _cbm_k_basin, _cbm_k_chrin


_cbm_k_basin:
_cbm_k_chrin:
        jsr     BASIN
        ldx     #0              ; Clear high byte
        rts
