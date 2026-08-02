;
; Oliver Schmidt, 17.04.2005
; Robert Justice, 2026
;
; unsigned char __fastcall__ _syschdir (const char* name);
;

        .export         __syschdir
        .import         pushname, popname, sos_set_pathname_tos
        .import         initcwd

        .include        "zeropage.inc"
        .include        "sos.inc"

__syschdir:
        ; Push name
        jsr     pushname
        bne     oserr

        ; Set pushed name
        jsr     sos_set_pathname_tos

        ; Change directory
        lda     #SET_PREFIX_CALL
        ldx     #SET_PREFIX_COUNT
        jsr     callsos
        bne     cleanup

        ; Update current working directory
        jsr     initcwd
        lda     #$00

        ; Cleanup name
cleanup:jsr     popname         ; Preserves A

oserr:  rts
