; ispunct.s
;
; This file is part of
; cc65 - a freeware C compiler for 6502 based systems
;
; https://cc65.github.io
;
; See "LICENSE" file for legal information.
;
; int ispunct (int c);
;

        .export         _ispunct
        .include        "ctype.inc"
        .import         ctypemask

_ispunct:
        jsr     ctypemask       ; (always clears X)
        bcs     @L1             ; out of range? (everything already clear -> false)
        and     #CT_NOT_PUNCT   ; mask relevant bits
        cmp     #1              ; if false, then set "borrow" flag
        lda     #0
        sbc     #0              ; invert logic (return NOT (space | control | digit | alpha))
@L1:    rts
