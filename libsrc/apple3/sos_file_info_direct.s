;
; Colin Leroy-Mira, 2023 <colin@colino.net>
; Robert Justice, 2026
;

        .export         sos_file_info_direct
        .include        "zeropage.inc"
        .include        "sos.inc"

        ; Calls SOS GET_FILE_INFO on the ProDOS style
        ; filename stored on top of stack
        ; Returns with carry set on error, and sets errno
sos_file_info_direct:
        ; Set pushed name from TOS
        lda     c_sp
        ldx     c_sp+1
        sta     sosparam + SOS::INFO::PATHNAME
        stx     sosparam + SOS::INFO::PATHNAME+1
        lda     #$0F            ; get all info
        sta     sosparam + SOS::INFO::LENGTH

        ; Get file information
        lda     #GET_INFO_CALL
        ldx     #GET_INFO_COUNT
        jmp     callsos
