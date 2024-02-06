;* cgetc

        .export         _cgetc
        .include        "creativision.inc"

_cgetc:
        lda     #$80

L1:     bit     ZP_KEYBOARD
        bpl     L1

        lda     ZP_KEYBOARD
        and     #$7F
        rts
