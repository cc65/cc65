; iscntrl.s
;
; This file is part of
; cc65 - a freeware C compiler for 6502 based systems
;
; https://cc65.github.io
;
; See "LICENSE" file for legal information.
;
; int iscntrl (int c);
;

        .export         _iscntrl
        .include        "ctype.inc"
        .import         ctypemask

_iscntrl:
        jsr     ctypemask      ; (always clears X)
        bcs     @L1                     ; out of range? (everything already clear -> false)
        and     #CT_CTRL                ; mask control character bit
@L1:    rts
