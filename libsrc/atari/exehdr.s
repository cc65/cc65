; This file defines the EXE header and main chunk load header for Atari executables

        .export         __EXEHDR__: absolute = 1
        .import         __MAIN_START__, __BSS_LOAD__

.segment        "EXEHDR"
        .word   $FFFF

.segment        "MAINHDR"
        .word   __MAIN_START__
        .word   __BSS_LOAD__ - 1
