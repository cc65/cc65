        .macpack longbranch

        .import _exit
        .export _main

        .macpack cpu
        .macro  pfinc   what
                .out .sprintf("Including bug%s.inc", what)
                .include .sprintf("bug%s.inc", what)
        .endmacro

_main:
        ldx #1  ; test counter

        pfinc   "1655"

        ;---------------------------------------------------------------------
        ; exit OK
        ldx #0
exiterror:
        txa
        ldx #0
        jmp _exit
