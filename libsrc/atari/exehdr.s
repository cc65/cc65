; This file defines the EXE header for Atari executables

        .export         __EXEHDR__: absolute = 1

.segment        "EXEHDR"

        .word   $FFFF

