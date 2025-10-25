;
; Colin Leroy-Mira, 2025 <colin@colino.net>
;

        .export         mli_set_pathname_tos
        .include        "zeropage.inc"
        .include        "mli.inc"

        ; Sets MLI PATHNAME parameter from TOS
mli_set_pathname_tos:
        ; Set pushed name from TOS
        lda     c_sp
        ldx     c_sp+1
        sta     mliparam + MLI::PATH::PATHNAME
        stx     mliparam + MLI::PATH::PATHNAME+1
        rts
