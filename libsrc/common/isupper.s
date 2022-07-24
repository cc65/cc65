; isupper.s
;
; This file is part of
; cc65 - a freeware C compiler for 6502 based systems
;
; https://cc65.github.io
;
; See "LICENSE" file for legal information.
;
; int isupper (int c);
;

        .export         _isupper
        .include        "ctype.inc"
        .import         ctypemask

_isupper:
        jsr     ctypemask       ; (always clears X)
        bcs     @L1             ; out of range? (everything already clear -> false)
        and     #CT_UPPER       ; mask upper char bit
@L1:    rts
