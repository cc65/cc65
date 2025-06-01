;
; Colin Leroy-Mira, 2023 <colin@colino.net>
;

        .export         mli_file_info_direct
        .include        "zeropage.inc"
        .include        "mli.inc"

        ; Calls ProDOS MLI GET_FILE_INFO on the ProDOS style
        ; filename stored on top of stack
        ; Returns with carry set on error, and sets errno
mli_file_info_direct:
        ; Set pushed name
        lda     sp
        ldx     sp+1
        sta     mliparam + MLI::INFO::PATHNAME
        stx     mliparam + MLI::INFO::PATHNAME+1

        ; Get file information
        lda     #GET_INFO_CALL
        ldx     #GET_INFO_COUNT
        jmp     callmli
