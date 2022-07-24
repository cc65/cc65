
    .export _cpeekcolor

    .include "c64.inc"

    .segment "CODE"

_cpeekcolor:

    ldy CURS_X
    lda (CRAM_PTR),y    ; get color
    and #$0f
    ldx #0
    rts
