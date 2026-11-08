; .END inside a macro or .REPEAT ends the assembly. It used to abort
; ca65 with "Check failed: IStack == 0".

        .byte   1
.macro  stop
        .byte   2
        .end
.endmacro
        stop
        .byte   3                       ; not assembled
