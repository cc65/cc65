; Define macro setting up ca65 string literals for C64 screen codes (see https://www.lemon64.com/forum/viewtopic.php?t=72236)

.macro SCREENCODE
    .repeat $20, i
        .charmap $40 + i, $40 + i + $00
    .endrepeat

    .repeat $20, i
        .charmap $60 + i, $60 + i - $60
    .endrepeat

    .repeat $20, i
        .charmap $80 + i, $80 + i + $40
    .endrepeat

    .repeat $20, i
        .charmap $A0 + i, $A0 + i - $40
    .endrepeat

    .repeat $3F, i
        .charmap $C0 + i, $C0 + i - $80
    .endrepeat

    .charmap $FF, $5E
.endmacro

SCREENCODE ; Apply C64 screen code character translation

;--------------------------------------------------------------------------------------------------

        jsr $e544

        ldx #text_ - text
loop:
        lda text-1, x
        sta $0400-1, x
        dex
        bne loop
        rts

;--------------------------------------------------------------------------------------------------

text:
        .byte "hello world!1"
text_:
