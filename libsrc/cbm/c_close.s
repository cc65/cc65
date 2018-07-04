;
; Ullrich von Bassewitz, 03.06.1999
;
; void __fastcall__ cbm_k_close (unsigned char FN);
;

        .include        "cbm.inc"

        .export         _cbm_k_close

_cbm_k_close:
        clc
        jmp     CLOSE
