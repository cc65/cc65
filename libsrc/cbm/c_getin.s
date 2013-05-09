;
; Ullrich von Bassewitz, 2012-07-01
;
; unsigned char cbm_k_getin (void);
;

        .export         _cbm_k_getin
        .import         GETIN


_cbm_k_getin:
        jsr     GETIN
        ldx     #0
        rts
