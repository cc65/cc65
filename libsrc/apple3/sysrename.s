;
; Oliver Schmidt, 15.04.2005
; Robert Justice, 2026
;
; unsigned char __fastcall__ _sysrename (const char* oldname, const char* newname);
;

        .export         __sysrename
        .import         pushname, pushname_tos, sos_set_pathname_tos, popname
        .import         popax

        .include        "zeropage.inc"
        .include        "sos.inc"

__sysrename:
        ; Save newname
        sta     ptr2
        stx     ptr2+1

        ; Get and push oldname
        jsr     pushname_tos
        bne     oserr1

        ; Save oldname
        lda     c_sp
        ldx     c_sp+1
        sta     ptr3
        stx     ptr3+1

        ; Restore and push newname
        lda     ptr2
        ldx     ptr2+1
        jsr     pushname
        bne     oserr2

        ; Set pushed oldname
        lda     ptr3
        ldx     ptr3+1
        sta     sosparam + SOS::RENAME::PATHNAME
        stx     sosparam + SOS::RENAME::PATHNAME+1

        ; Set pushed newname
        lda     c_sp
        ldx     c_sp+1
        sta     sosparam + SOS::RENAME::NEW_PATHNAME
        stx     sosparam + SOS::RENAME::NEW_PATHNAME+1

        ; Rename file
        lda     #RENAME_CALL
        ldx     #RENAME_COUNT
        jsr     callsos

        ; Cleanup newname
        jsr     popname         ; Preserves A

        ; Cleanup oldname
oserr2: jmp     popname         ; Preserves A

oserr1: rts
