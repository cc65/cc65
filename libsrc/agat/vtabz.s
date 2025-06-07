;
; Oleg A. Odintsov, Moscow, 2024
;
; VTABZ routine
;

    .export     VTABZ
    .include    "agat.inc"

VTABZ:
    lda    CV
    ror
    ror
    ror
    and    #$C0
    sta    BASL
    lda    CV
    lsr
    lsr
    eor    BASH
    and    #$07
    eor    BASH
    sta    BASH
    rts
