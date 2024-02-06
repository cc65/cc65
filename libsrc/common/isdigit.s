; isdigit.s
;
; This file is part of
; cc65 - a freeware C compiler for 6502 based systems
;
; https://cc65.github.io
;
; See "LICENSE" file for legal information.
;
; int isdigit (int c);
;

        .export         _isdigit
        .include        "ctype.inc"
        .import         ctypemask

_isdigit:
        jsr     ctypemask      ; (always clears X)
        bcs     @L1                     ; out of range? (everything already clear -> false)
        and     #CT_DIGIT               ; mask digit bit
@L1:    rts
