        .export         vce_init

        .include        "pce.inc"

        .segment        "ONCE"
vce_init:
        ; Set CTA to zero
        stz     VCE_ADDR_LO
        stz     VCE_ADDR_HI

        ldy     #$01            ; Only background palettes
vce_clear_bank:
        clx                     ; ldx #<$0100 ; <(16 * 16)
vce_clear_color:
        stz     VCE_DATA_LO     ; Clear color (LSB)
        stz     VCE_DATA_HI     ; Clear color (MSB)
        dex
        bne     vce_clear_color
        dey
        bne     vce_clear_bank
        rts
