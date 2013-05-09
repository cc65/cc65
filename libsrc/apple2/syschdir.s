;
; Oliver Schmidt, 17.04.2005
;
; unsigned char __fastcall__ _syschdir (const char* name);
;

        .export         __syschdir
        .import         pushname, popname
        .import         initcwd

        .include        "zeropage.inc"
        .include        "mli.inc"

__syschdir:
        ; Push name
        jsr     pushname
        bne     oserr

        ; Set pushed name
        lda     sp
        ldx     sp+1
        sta     mliparam + MLI::PREFIX::PATHNAME
        stx     mliparam + MLI::PREFIX::PATHNAME+1

        ; Change directory
        lda     #SET_PREFIX_CALL
        ldx     #PREFIX_COUNT
        jsr     callmli
        bcs     cleanup

        ; Update current working directory
        jsr     initcwd         ; Returns with A = 0

        ; Cleanup name
cleanup:jsr     popname         ; Preserves A

oserr:  rts
