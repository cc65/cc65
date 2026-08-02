;
; Oliver Schmidt, 15.04.2005
; Robert Justice, 2026
;
; unsigned char _sysmkdir (const char* name, ...);
;

        .export         __sysmkdir
        .import         pushname_tos, popname, sos_set_pathname_tos
        .import         addysp, popax

        .include        "zeropage.inc"
        .include        "sos.inc"

__sysmkdir:
        ; Throw away all parameters except the name
        dey
        dey
        jsr     addysp

        ; Get and push name
        jsr     pushname_tos
        bne     oserr

        ; Set pushed name
        jsr     sos_set_pathname_tos

        ; Set all other parameters from template
        ldx     #(OPTION::CREATE::STORAGE_TYPE+1) - (OPTION::CREATE::FILE_TYPE)
        stx     sosparam + SOS::CREATE::LENGTH     ; length in bytes
        dex
:       lda     CREATE,x
        sta     sosoption + OPTION::CREATE::FILE_TYPE,x
        dex
        bpl     :-

        ; Make directory
        lda     #CREATE_CALL
        ldx     #CREATE_COUNT
        jsr     callsos

        ; Cleanup name
        jsr     popname         ; Preserves A

oserr:  rts

        .rodata

CREATE: .byte   $0F             ; FILE_TYPE:    Directory file
        .word   $0000           ; AUX_TYPE:     N/A
        .byte   $0D             ; STORAGE_TYPE: Linked directory file
