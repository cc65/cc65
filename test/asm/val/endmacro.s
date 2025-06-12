; for PR #2013
    .import _exit
    .export _main

    ; this macro is invalid, but should not cause an error (if it is never expanded)
    .macro invalid
        nop .endmacro
    .endmacro

    .define temp_endmac .endmacro
    .macro new_mac
        .define startmac .macro
        .define endmac .endmacro
    temp_endmac

    .undefine temp_endmac

    new_mac

    startmac dex2
        dex
        dex
    endmac

_main:
    ldx #$02
    dex2
    ; x should be zero
    txa
    jmp _exit
