
    .export _cpeekcolor

    .include "plus4.inc"

    .segment "CODE"

_cpeekcolor:

    ldy CURS_X
    lda (CRAM_PTR),y    ; get color
    ;and #$0f is this ok?
    ldx #0
    rts
