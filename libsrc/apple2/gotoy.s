;
; Ullrich von Bassewitz, 06.08.1998
;
; void __fastcall__ gotoy (unsigned char y);
;

        .export         _gotoy
        .import         VTABZ

        .include        "apple2.inc"

_gotoy:
        clc
        adc     WNDTOP
        sta     CV              ; Store Y
        jmp     VTABZ


