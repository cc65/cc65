; isxdigit.s
;
; This file is part of
; cc65 - a freeware C compiler for 6502 based systems
;
; https://cc65.github.io
;
; See "LICENSE" file for legal information.
;
; int isxdigit (int c);
;

        .export         _isxdigit
        .include        "ctype.inc"
        .import         ctypemask

_isxdigit:
        jsr     ctypemask       ; (always clears X)
        bcs     @L1             ; out of range? (everything already clear -> false)
        and     #CT_XDIGIT      ; mask xdigit bit
@L1:    rts
