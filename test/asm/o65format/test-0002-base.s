        .import IMPORTED
        .export EXPORTED

        jmp EXPORTED
start:
        ldx #>(IMPORTED+$89AB)
        ldy #<(IMPORTED+$89AB)
EXPORTED:
        jmp IMPORTED

end:
