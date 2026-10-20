; .END inside .REPEAT, see 315-end-in-macro.s

        .byte   1
.repeat 2
        .byte   2
        .end
.endrep
        .byte   3                       ; not assembled
