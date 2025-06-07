;
; Ullrich von Bassewitz, 06.08.1998
; Oleg A. Odintsov, Moscow, 2024
;
; void __fastcall__ gotoy (unsigned char y);
;

    .import     VTABZ
    .export     _gotoy
    .include    "agat.inc"

_gotoy:
    clc
    adc    WNDTOP
    sta    CV
    jmp    VTABZ
