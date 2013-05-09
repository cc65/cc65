;
; Ullrich von Bassewitz, 03.06.1999
;
; void __fastcall__ cbm_k_setlfs (unsigned char LFN,
;                                 unsigned char DEV,
;                                 unsigned char SA);
;

        .export         _cbm_k_setlfs
        .import         SETLFS
        .import         popa
        .importzp       tmp1
                              

_cbm_k_setlfs:
        sta     tmp1            ; Save SA
        jsr     popa            ; Get DEV
        tax
        jsr     popa            ; Get LFN
        ldy     tmp1            ; Get SA
        jmp     SETLFS


