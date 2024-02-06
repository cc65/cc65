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
        .import         ctypemaskdirect

_toupper:
        cpx     #$00            ; out of range?
        bne     @L2             ; if so, return the argument unchanged
        tay                     ; save char
        jsr     ctypemaskdirect ; get character classification
        and     #CT_LOWER       ; lower case char?
        beq     @L1             ; jump if no
        tya                     ; restore char
        adc     #<('A'-'a')     ; make upper case char (ctypemaskdirect ensures carry clear)
        rts
@L1:    tya                     ; restore char
@L2:    rts
