;
; Ullrich von Bassewitz, 03.06.1999
;
; void __fastcall__ cbm_k_close (unsigned char FN);
;

        .export         _cbm_k_close
        .import         CLOSE

_cbm_k_close:
        clc                  
        jmp     CLOSE


