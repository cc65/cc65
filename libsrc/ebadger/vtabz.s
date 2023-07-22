;
; Oliver Schmidt, 03.03.2007
;
; VTABZ routine
;

        .export         VTABZ

        .segment        "LOWCODE"
        .include "ebadger.inc"

VTABZ:
        ; Accumulator holds the row
        ; Calculate the line address for text mode
        phx
        tax

        ; start at first row
        LDA #$00
        STA BASL
        LDA #$04
        STA BASH

        cpx #$00
        beq :+

@loop:
        clc
        lda BASL
        adc #$28
        sta BASL
        lda BASH
        adc #$0
        sta BASH
        dex
        bpl @loop
:
        plx
        rts
