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
        .import         ctype_preprocessor_no_check

_tolower:
        cpx     #$00            ; out of range?
        bne     @L2             ; if so, return the argument unchanged
        tay                     ; save char
        jsr     ctype_preprocessor_no_check
        and     #CT_UPPER       ; upper case char?
        beq     @L1             ; jump if no
        tya                     ; restore char
        adc     #<('a'-'A')     ; make lower case char (ctype_preprocessor_no_check ensures carry clear)
        rts
@L1:    tya                     ; restore char
@L2:    rts
