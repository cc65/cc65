;
; Oliver Schmidt, 17.04.2005
;
; unsigned char __fastcall__ _syschdir (const char* name);
;

        .export         __syschdir
        .import         pushname, popname, mli_set_pathname_tos
        .import         initcwd

        .include        "zeropage.inc"
        .include        "mli.inc"

__syschdir:
        ; Push name
        jsr     pushname
        bne     oserr

        ; Set pushed name
        jsr     mli_set_pathname_tos

        ; Change directory
        lda     #SET_PREFIX_CALL
        ldx     #PREFIX_COUNT
        jsr     callmli
        bcs     cleanup

        ; Update current working directory
        jsr     initcwd
        lda     #$00

        ; Cleanup name
cleanup:jsr     popname         ; Preserves A

oserr:  rts
