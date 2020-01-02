; isdigit.s
;
; This file is part of
; cc65 - a freeware C compiler for 6502 based systems
;
; https://github.com/cc65/cc65
;
; See "LICENSE" file for legal information.
;
; int isdigit (int c);
;

        .export         _isdigit
        .include        "ctype.inc"
        .import         ctype_preprocessor

_isdigit:
        jsr     ctype_preprocessor      ; (clears always x)
        bcs     @L1                     ; out of range? (everything already clear -> false)
        and     #CT_DIGIT               ; mask digit bit
 @L1:   rts
