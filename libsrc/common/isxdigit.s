; isxdigit.s
;
; This file is part of
; cc65 - a freeware C compiler for 6502 based systems
;
; https://github.com/cc65/cc65
;
; See "LICENSE" file for legal information.
;
; int isxdigit (int c);
;

        .export         _isxdigit
        .include        "ctype.inc"
        .import         ctype_preprocessor        

_isxdigit:
        jsr     ctype_preprocessor      ; (clears always x)
        bcs     @L1                     ; out of range? (everything already clear -> false)
        and     #CT_XDIGIT              ; mask xdigit bit
@L1:    rts
