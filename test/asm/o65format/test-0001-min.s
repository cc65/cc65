        .import IMPORTED

start:
        ldx #>(IMPORTED+$89AB)
        ldy #<(IMPORTED+$89AB)
        jmp IMPORTED

end:
