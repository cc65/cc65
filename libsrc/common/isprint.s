; isprint.s
;
; This file is part of
; cc65 - a freeware C compiler for 6502 based systems
;
; https://cc65.github.io
;
; See "LICENSE" file for legal information.
;
; int isprint (int c);
;

        .export         _isprint
        .include        "ctype.inc"
        .import         ctypemask

_isprint:
        jsr     ctypemask       ; (always clears X)
        bcs     @L1             ; out of range? (everything already clear -> false)
        eor     #CT_CTRL        ; NOT a control char
        and     #CT_CTRL        ; mask control char bit
@L1:    rts
