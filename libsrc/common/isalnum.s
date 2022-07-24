; isalnum.s
;
; This file is part of
; cc65 - a freeware C compiler for 6502 based systems
;
; https://cc65.github.io
;
; See "LICENSE" file for legal information.
;
; int isalnum (int c);
;

        .export         _isalnum
        .include        "ctype.inc"
        .import         ctypemask

_isalnum:
        jsr     ctypemask      ; (always clears X)
        bcs     @L1                     ; out of range? (everything already clear -> false)
        and     #CT_ALNUM               ; mask character/digit bits
@L1:    rts
