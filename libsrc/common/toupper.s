; toupper.s
;
; This file is part of
; cc65 - a freeware C compiler for 6502 based systems
;
; https://cc65.github.io
;
; See "LICENSE" file for legal information.
;
; int toupper (int c);
;

        .export         _toupper
        .include        "ctype.inc"
        .import         ctype_preprocessor

_toupper:
        tay                             ; save char
        jsr     ctype_preprocessor      ; (always clears X)
        bcc     @L2                     ; out of range?
@L1:    tya                             ; if so, return the argument unchanged
        rts
@L2:    and     #CT_LOWER               ; lower case char?
        beq     @L1                     ; jump if no
        tya                             ; restore char
        adc     #<('A'-'a')             ; make upper case char (ctype_preprocessor ensures carry clear)
        rts
