; tolower.s
;
; This file is part of
; cc65 - a freeware C compiler for 6502 based systems
;
; https://cc65.github.io
;
; See "LICENSE" file for legal information.
;
; int tolower (int c);
;

        .export         _tolower
        .include        "ctype.inc"
        .import         ctype_preprocessor

_tolower:
        tay                             ; save char
        jsr     ctype_preprocessor      ; (always clears X)
        bcc     @L2                     ; out of range?
@L1:    tya                             ; if so, return the argument unchanged
        rts
@L2:    and     #CT_UPPER               ; upper case char?
        beq     @L1                     ; jump if no
        tya                             ; restore char
        adc     #<('a'-'A')             ; make lower case char (ctype_preprocessor ensures carry clear)
        rts
