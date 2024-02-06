;
; Oliver Schmidt, 15.04.2005
;
; unsigned char __fastcall__ _sysremove (const char* name);
;

        .export         __sysremove
        .import         pushname, popname

        .include        "zeropage.inc"
        .include        "mli.inc"

__sysremove:
        ; Push name
        jsr     pushname
        bne     oserr

        ; Set pushed name
        lda     sp
        ldx     sp+1
        sta     mliparam + MLI::DESTROY::PATHNAME
        stx     mliparam + MLI::DESTROY::PATHNAME+1

        ; Remove file
        lda     #DESTROY_CALL
        ldx     #DESTROY_COUNT
        jsr     callmli

        ; Cleanup name
        jsr     popname         ; Preserves A

oserr:  rts
