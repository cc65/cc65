;
; Oliver Schmidt, 15.04.2005
;
; unsigned char __fastcall__ _sysrename (const char* oldname, const char* newname);
;

        .export         __sysrename
        .import         pushname, pushname_tos, mli_set_pathname_tos, popname
        .import         popax

        .include        "zeropage.inc"
        .include        "mli.inc"

__sysrename:
        ; Save newname
        sta     ptr2
        stx     ptr2+1

        ; Get and push oldname
        jsr     pushname_tos
        bne     oserr1

        ; Set pushed oldname
        jsr     mli_set_pathname_tos

        ; Restore and push newname
        lda     ptr2
        ldx     ptr2+1
        jsr     pushname
        bne     oserr2

        ; Set pushed newname
        lda     c_sp
        ldx     c_sp+1
        sta     mliparam + MLI::RENAME::NEW_PATHNAME
        stx     mliparam + MLI::RENAME::NEW_PATHNAME+1

        ; Rename file
        lda     #RENAME_CALL
        ldx     #RENAME_COUNT
        jsr     callmli

        ; Cleanup newname
        jsr     popname         ; Preserves A

        ; Cleanup oldname
oserr2: jmp     popname         ; Preserves A

oserr1: rts
