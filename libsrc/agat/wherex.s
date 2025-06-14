;
; Kevin Ruland
; Oleg A. Odintsov, Moscow, 2024
;
; unsigned char wherex (void);
;

        .export         _wherex

        .include        "agat.inc"

_wherex:
    lda    CH
    bit    TATTR
    bmi    t64
    lsr
t64:
    ldx     #$00
    rts
