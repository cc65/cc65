;
; Colin Leroy-Mira, 2025 <colin@colino.net>
;
; int __fastcall__ file_set_type(const char *pathname, unsigned char type);
; int __fastcall__ file_set_auxtype(const char *pathname, unsigned int auxtype);
;

        .export         _file_set_type, _file_set_auxtype
        .import         pushname_tos, popname, mli_file_info_direct
        .import         popa, popax
        .include        "zeropage.inc"
        .include        "errno.inc"
        .include        "mli.inc"

new_value      = ptr2     ; ptr1 is used by pushname_tos
mod_flag       = tmp1

UPDATE_TYPE    = $00
UPDATE_AUXTYPE = $80

_file_set_type:
        sta     new_value
        ldy     #UPDATE_TYPE
        beq     mli_update
_file_set_auxtype:
        sta     new_value
        stx     new_value+1
        ldy     #UPDATE_AUXTYPE

mli_update:
        sty     mod_flag

        ; Get pathname
        jsr     pushname_tos
        bne     oserr

        ; ProDOS 8 TechRef, 4.4.4: You should use
        ; the GET_FILE_INFO call to read a file’s
        ; attributes into a parameter list, modify
        ; them as needed, and then use the same
        ; parameter list for the SET_FILE_INFO call.
        jsr     mli_file_info_direct
        ; Bail if we could not get the information.
        bcs     cleanup

        ; Update type if needed
        bit     mod_flag
        bmi     :+
        lda     new_value
        sta     mliparam + MLI::INFO::FILE_TYPE
        jmp     set_info

:       ; Otherwise update auxtype
        lda     new_value
        sta     mliparam + MLI::INFO::AUX_TYPE
        lda     new_value+1
        sta     mliparam + MLI::INFO::AUX_TYPE+1

set_info:
        ; Set file information
        lda     #SET_INFO_CALL
        ldx     #SET_INFO_COUNT
        jsr     callmli

cleanup:
        php                     ; Save return status

        jsr     popname         ; Preserves A

        plp
        bcs     oserr
        rts

oserr:
        jsr     ___mappederrno
        sec
        rts
