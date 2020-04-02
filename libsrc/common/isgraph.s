; isgraph.s
;
; This file is part of
; cc65 - a freeware C compiler for 6502 based systems
;
; https://cc65.github.io
;
; See "LICENSE" file for legal information.
;
; int isgraph (int c);
;

        .export         _isgraph
        .include        "ctype.inc"
        .import         ctypemask

_isgraph:
        jsr     ctypemask      ; (always clears X)
        bcs     @L1                     ; out of range? (everything already clear -> false)
        and     #CT_CTRL_SPACE          ; mask character bits
        cmp     #1                      ; if false, then set "borrow" flag
        lda     #0
        sbc     #0                      ; invert logic (return NOT control and NOT space)
@L1:    rts
