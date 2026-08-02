;
; Colin Leroy-Mira, 2023 <colin@colino.net>
; Robert Justice, 2026
;

        .export         sos_file_info
        .import         pushname_tos, popname, sos_file_info_direct
        .import         popax
        .include        "zeropage.inc"
        .include        "errno.inc"
        .include        "sos.inc"

        ; Calls SOS GET_FILE_INFO on the filename
        ; stored as C string in AX at top of stack
        ; Returns with carry set on error, and sets errno
sos_file_info:
        ; Get pathname from top of stack
        jsr     pushname_tos
        bne     oserr

        jsr     sos_file_info_direct
        php                     ; Save return status

        jsr     popname         ; Preserves A

        plp
        bne     oserr
        rts

oserr:
        jsr     ___mappederrno
        sec
        rts
