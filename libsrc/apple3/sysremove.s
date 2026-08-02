;
; Oliver Schmidt, 15.04.2005
; Robert Justice, 2026
;
; unsigned char __fastcall__ _sysremove (const char* name);
;

        .export         __sysremove
        .import         pushname, popname, sos_set_pathname_tos

        .include        "zeropage.inc"
        .include        "sos.inc"

__sysremove:
        ; Push name
        jsr     pushname
        bne     oserr

        ; Set pushed name
        jsr     sos_set_pathname_tos

        ; Remove file
        lda     #DESTROY_CALL
        ldx     #DESTROY_COUNT
        jsr     callsos

        ; Cleanup name
        jsr     popname         ; Preserves A

oserr:  rts
