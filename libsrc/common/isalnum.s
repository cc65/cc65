; isalnum.s
;
; This file is part of
; cc65 - a freeware C compiler for 6502 based systems
;
; https://github.com/cc65/cc65
;
; See "LICENSE" file for legal information.
;
; int isalnum (int c);
;

        .export         _isalnum
        .include        "ctype.inc"
        .import         ctype_preprocessor

_isalnum:
        jsr     ctype_preprocessor      ; (clears always x)
        bcs     @L1                     ; out of range? (everything already clear -> false)
        and     #CT_ALNUM               ; mask character/digit bits
@L1:    rts
