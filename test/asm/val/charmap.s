    .setcpu "6502"

    .import _exit
    .export _main

    .charmap $41, 0
    .charmap $42, 1
    .charmap $43, 2
mapped:
    .byte "ABC"

    .include "ascii_charmap.inc"
ascii:
    .byte "ABC"

_main:
    ldx #$01

    lda mapped+0
    cmp #0
    bne error
    lda mapped+1
    cmp #1
    bne error
    lda mapped+2
    cmp #2
    bne error

    lda ascii+0
    cmp #$41
    bne error
    lda ascii+1
    cmp #$42
    bne error
    lda ascii+2
    cmp #$43
    bne error

    ldx #$00

error:
    txa
    jmp _exit
