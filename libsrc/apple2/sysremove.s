;
; Oliver Schmidt, 15.04.2005
;
; unsigned char __fastcall__ _sysremove (const char* name);
;

        .export         __sysremove
        .import         pushname, popname, mli_set_pathname_tos

        .include        "zeropage.inc"
        .include        "mli.inc"

__sysremove:
        ; Push name
        jsr     pushname
        bne     oserr

        ; Set pushed name
        jsr     mli_set_pathname_tos

        ; Remove file
        lda     #DESTROY_CALL
        ldx     #DESTROY_COUNT
        jsr     callmli

        ; Cleanup name
        jsr     popname         ; Preserves A

oserr:  rts
