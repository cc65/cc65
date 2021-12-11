; isspace.s
;
; This file is part of
; cc65 - a freeware C compiler for 6502 based systems
;
; https://cc65.github.io
;
; See "LICENSE" file for legal information.
;
; int isspace (int c);
;

        .export         _isspace
        .include        "ctype.inc"
        .import         ctypemask

_isspace:
        jsr     ctypemask               ; (always clears X)
        bcs     @L1                     ; out of range? (everything already clear -> false)
        and     #(CT_SPACE | CT_OTHER_WS) ; mask space bits
@L1:    rts
