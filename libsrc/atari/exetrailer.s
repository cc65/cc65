; This file defines the EXE file "trailer" which sets the entry point

        .export         __AUTOSTART__: absolute = 1
        .import         start

        .include        "atari.inc"

.segment "AUTOSTRT"
        .word   RUNAD                   ; defined in atari.inc
        .word   RUNAD+1
        .word   start

