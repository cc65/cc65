;
; Oliver Schmidt, 15.04.2005
;
; unsigned char _sysmkdir (const char* name, ...);
;

        .export         __sysmkdir
        .import         pushname, popname
        .import         addysp, popax

        .include        "zeropage.inc"
        .include        "mli.inc"

__sysmkdir:
        ; Throw away all parameters except the name
        dey
        dey
        jsr     addysp

        ; Get and push name
        jsr     popax
        jsr     pushname
        bne     oserr

        ; Set pushed name
        lda     sp
        ldx     sp+1
        sta     mliparam + MLI::CREATE::PATHNAME
        stx     mliparam + MLI::CREATE::PATHNAME+1

        ; Set all other parameters from template
        ldx     #(MLI::CREATE::CREATE_TIME+1) - (MLI::CREATE::PATHNAME+1) - 1
:       lda     CREATE,x
        sta     mliparam + MLI::CREATE::ACCESS,x
        dex
        bpl     :-

        ; Make directory
        lda     #CREATE_CALL
        ldx     #CREATE_COUNT
        jsr     callmli

        ; Cleanup name
        jsr     popname         ; Preserves A

oserr:  rts

        .rodata

CREATE: .byte   %11000011       ; ACCESS:       Standard full access
        .byte   $0F             ; FILE_TYPE:    Directory file
        .word   $0000           ; AUX_TYPE:     N/A
        .byte   $0D             ; STORAGE_TYPE: Linked directory file
        .word   $0000           ; CREATE_DATE:  Current date
        .word   $0000           ; CREATE_TIME:  Current time
