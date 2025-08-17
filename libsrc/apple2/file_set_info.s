;
; Colin Leroy-Mira, 2025 <colin@colino.net>
;
; int __fastcall__ file_set_type(const char *pathname, unsigned char type);
; int __fastcall__ file_set_auxtype(const char *pathname, unsigned int auxtype);
;

        .export         _file_set_type, _file_set_auxtype
        .import         pushname, popname, mli_file_info_direct
        .import         popa, popax
        .include        "zeropage.inc"
        .include        "errno.inc"
        .include        "mli.inc"

auxtype        = tmp1
type           = tmp3
mod_flag       = tmp4

UPDATE_TYPE    = $00
UPDATE_AUXTYPE = $FF

_file_set_type:
        sta     type
        ldy     #UPDATE_TYPE
        sty     mod_flag
        beq     mli_update
_file_set_auxtype:
        sta     auxtype
        stx     auxtype+1
        ldy     #UPDATE_AUXTYPE
        sty     mod_flag

mli_update:
        ; Get pathname
        jsr     popax
        jsr     pushname
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
        lda     type
        sta     mliparam + MLI::INFO::FILE_TYPE
        jmp     set_info

:       ; Otherwise update auxtype
        lda     auxtype
        sta     mliparam + MLI::INFO::AUX_TYPE
        lda     auxtype+1
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
