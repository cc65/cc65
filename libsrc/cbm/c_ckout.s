;
; Ullrich von Bassewitz, 03.06.1999
;
; unsigned char __fastcall__ cbm_k_ckout (unsigned char FN);
;

        .include        "cbm.inc"

        .export         _cbm_k_ckout


_cbm_k_ckout:
        tax
        jsr     CKOUT
        ldx     #0              ; Clear high byte
        bcs     @NotOk
        txa
@NotOk: rts
