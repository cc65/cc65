; isascii.s
;
; This file is part of
; cc65 - a freeware C compiler for 6502 based systems
;
; https://cc65.github.io
;
; See "LICENSE" file for legal information.
;
; int isascii (int c);
;

        .export         _isascii
        .import         return0

_isascii:
        asl     a               ; high-bit to carry
        txa                     ; check range of input param
        bne     @L1             ; out-of bounds?
        adc     #$FF            ; calculate return value based on carry
        rts

@L1:    jmp     return0         ; return false
