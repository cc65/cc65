;
; Ullrich von Bassewitz, 19.11.2002
;
; C128 Kernal functions
;

        .include "cbm_kernal.inc"

        .export         KBDREAD
        .export         CLRSCR
        .export         PRINT
        .export         NEWLINE
        .export         CURS_SET
        .export         CURS_ON
        .export         CURS_OFF
        .export         NMIEXIT

        .export         C64MODE
        .export         SWAPPER
        .export         SETBNK

        .export         CINT
        .export         IOINIT
        .export         RAMTAS
        .export         RESTOR
        .export         VECTOR
        .export         SETMSG
        .export         SECOND
        .export         TKSA
        .export         MEMTOP
        .export         MEMBOT
        .export         SCNKEY
        .export         SETTMO
        .export         ACPTR
        .export         CIOUT
        .export         UNTLK
        .export         UNLSN
        .export         LISTEN
        .export         TALK
        .export         READST
        .export         SETLFS
        .export         SETNAM
        .export         OPEN
        .export         CLOSE
        .export         CHKIN
        .export         CKOUT
        .export         CLRCH
        .export         BASIN
        .export         CHRIN
        .export         BSOUT
        .export         CHROUT
        .export         LOAD
        .export         SAVE
        .export         SETTIM
        .export         RDTIM
        .export         STOP
        .export         GETIN
        .export         CLALL
        .export         UDTIM
        .export         SCREEN
        .export         PLOT
        .export         IOBASE
