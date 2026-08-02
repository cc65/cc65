;
; Colin Leroy-Mira, 2023 <colin@colino.net>
; Robert Justice, 2026
;

        .export         sos_set_pathname_tos
        .include        "zeropage.inc"
        .include        "sos.inc"

        ; Sets SOS PATHNAME parameter from TOS
sos_set_pathname_tos:
        ; Set pushed name from TOS
        lda     c_sp
        ldx     c_sp+1
        sta     sosparam + SOS::PATH::PATHNAME
        stx     sosparam + SOS::PATH::PATHNAME+1
        rts
