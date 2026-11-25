        .import IMPORTED
        .export EXPORTED
        .export end
        .export start
        .export j
        .export more_end
        .export dataref
        .export VIA

        VIA = $4000

        jmp EXPORTED
start:
        ldx #>(IMPORTED+$89AB)
        ldy #<(IMPORTED+$89AB)
EXPORTED:
        jmp IMPORTED

end:

.code
        lda #0
j:
        jmp $1234

more_end:
        rts

.data
        .byte <(IMPORTED+$1234)
        .byte >(IMPORTED+$1234)
dataref:
        .word (IMPORTED+$1234)
