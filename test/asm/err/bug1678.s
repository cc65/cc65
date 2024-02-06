
; #1678 Endless loop printing error messages

        .macpack longbranch

        .import _exit
        .export _main

.macro  foo
        foo
.endmacro

.struct bar
        foo
.endstruct

_main:
        ; exit OK
        lda #0
        ldx #0
        jmp _exit

