; isalpha.s
;
; This file is part of
; cc65 - a freeware C compiler for 6502 based systems
;
; https://cc65.github.io
;
; See "LICENSE" file for legal information.
;
; int isalpha (int c);
;

        .export         _isalpha
        .include        "ctype.inc"
        .import         ctypemask

_isalpha:
        jsr     ctypemask      ; (always clears X)
        bcs     @L1                     ; out of range? (everything already clear -> false)
        and     #CT_ALPHA               ; mask character bits
@L1:    rts
