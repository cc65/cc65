;
; Ullrich von Bassewitz, 2012-07-01
;
; unsigned char cbm_k_getin (void);
;

        .include        "cbm.inc"

        .export         _cbm_k_getin

_cbm_k_getin:
        jsr     GETIN
        ldx     #0
        rts
