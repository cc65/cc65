;
; Colin Leroy-Mira, 2023 <colin@colino.net>
;

        .export         mli_file_info
        .import         pushname, popname, mli_file_info_direct
        .import         popax
        .include        "zeropage.inc"
        .include        "errno.inc"
        .include        "mli.inc"

        ; Calls ProDOS MLI GET_FILE_INFO on the filename
        ; stored as C string in AX at top of stack
        ; Returns with carry set on error, and sets errno
mli_file_info:
        ; Get pathname
        jsr     popax
        jsr     pushname
        bne     oserr

        jsr     mli_file_info_direct
        php                     ; Save return status

        jsr     popname         ; Preserves A

        plp
        bcs     oserr
        rts

oserr:
        jsr     ___mappederrno
        sec
        rts
