
                        .include "pce.inc"

                        .export vce_init

vce_init:
                        stz     VCE_ADDR_LO   ;
                        stz     VCE_ADDR_HI   ; Set CTA to zero
                        ldy     #$01
vce_clear_bank:         ldx     #$00
vce_clear_color:        stz     VCE_DATA_LO   ; Clear color (LSB)
                        stz     VCE_DATA_HI   ; Clear color (MSB)
                        dex
                        bne     vce_clear_color
                        dey
                        bne     vce_clear_bank
                        rts
