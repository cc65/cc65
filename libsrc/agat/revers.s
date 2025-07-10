;
; Ullrich von Bassewitz, 2005-03-28
; Oleg A. Odintsov, Moscow, 2024
;
; unsigned char __fastcall__ revers (unsigned char onoff)
; unsigned char __fastcall__ flash (unsigned char onoff)
;

    .export    _revers, _flash

    .include    "agat.inc"

_revers:
    tax
    beq    noinv
    lda    TATTR
    and    #$D7
    sta    TATTR
    rts
noinv:
    lda    TATTR
    ora    #$20
    sta    TATTR
    rts

_flash:
    tax
    beq    noflash
    lda    TATTR
    and    #$DF
    ora    #$08
    sta    TATTR
    rts
noflash:
    lda    TATTR
    ora    #$20
    sta    TATTR
    rts
