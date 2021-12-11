;
; Ullrich von Bassewitz, 03.06.1999
;
; unsigned char cbm_k_open (void);
;

        .include        "cbm.inc"

        .export         _cbm_k_open


_cbm_k_open:
        jsr     OPEN
        ldx     #0              ; Clear high byte
        bcs     @NotOk
        txa
@NotOk: rts
