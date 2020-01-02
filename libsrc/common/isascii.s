; isascii.s
;
; This file is part of
; cc65 - a freeware C compiler for 6502 based systems
;
; https://github.com/cc65/cc65
;
; See "LICENSE" file for legal information.
;
; int isascii (int c);
;

        .export         _isascii

_isascii:
        cpx     #$00            ; Char range ok?
        bne     @L1             ; Jump if no

        tay
        bmi     @L1

        inx
        rts

@L1:    lda     #$00            ; Return false
        tax
        rts
