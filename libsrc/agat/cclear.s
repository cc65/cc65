;
; Oleg A. Odintsov, Moscow, 2024
;
; void __fastcall__ cclear (unsigned char length);
;

    .export     _cclear
    .import     COUT
    .include    "zeropage.inc"

_cclear:
    sta    ptr1
    lda    #$A0
next:    jsr COUT
    dec    ptr1
    bne    next
    rts
