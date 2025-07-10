; This file defines the EXE header and main chunk load header for Atari executables

        .export         __EXEHDR__: absolute = 1
        .import         __MAIN_START__, __INIT_LOAD__

.segment        "EXEHDR"
        .word   $FFFF

.segment        "MAINHDR"
        .word   __MAIN_START__
        .word   __INIT_LOAD__ - 1

; Define the INIT segment so that __INIT_LOAD__ from above '.import' is always defined.
; The segment is normally present when linking a C program, but not necessarily when linking an assembler program.
.segment        "INIT"
