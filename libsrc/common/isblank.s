; isblank.s
;
; This file is part of
; cc65 - a freeware C compiler for 6502 based systems
;
; https://cc65.github.io
;
; See "LICENSE" file for legal information.
;
; int isblank (int c);
;
; cc65 (and GNU) extension.
;

        .export         _isblank
        .include        "ctype.inc"
        .import         ctypemask

_isblank:
        jsr     ctypemask      ; (always clears X)
        bcs     @L1                     ; out of range? (everything already clear -> false)
        and     #CT_SPACE_TAB           ; mask blank bit
@L1:    rts
