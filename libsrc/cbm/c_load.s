;
; Ullrich von Bassewitz, 03.06.1999
;
; unsigned int __fastcall__ cbm_k_load (unsigned char flag, unsigned addr);
;

        .export         _cbm_k_load
        .import         LOAD
        .import         __oserror
        .import         popa
        .importzp       ptr1

_cbm_k_load:
        sta     ptr1
        stx     ptr1+1
        jsr     popa            ; get flag
        ldx     ptr1
        ldy     ptr1+1
        jsr     LOAD
        bcc     @Ok
        sta     __oserror
        ldx     ptr1
        ldy     ptr1+1
@Ok:    txa
        pha
        tya
        tax
        pla
        rts

