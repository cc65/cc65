; isblank.s
;
; This file is part of
; cc65 - a freeware C compiler for 6502 based systems
;
; https://github.com/cc65/cc65
;
; See "LICENSE" file for legal information.
;
; int isblank (int c);
;
; cc65 (and GNU) extension.
;

        .export         _isblank
        .include        "ctype.inc"
        .import         ctype_preprocessor     

_isblank:
        jsr     ctype_preprocessor      ; (clears always x)
        bcs     @L1                     ; out of range? (everything already clear -> false)
        and     #CT_SPACE_TAB           ; mask blank bit
 @L1:   rts
