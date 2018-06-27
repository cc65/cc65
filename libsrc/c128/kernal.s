;
; Ullrich von Bassewitz, 19.11.2002
;
; C128 kernal functions
;

	.include "cbmkernal.inc"

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
        .export         BSOUT
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


;-----------------------------------------------------------------------------
; All functions are available in the kernal jump table

; Extended jump table
C64MODE         = $FF4D
SWAPPER         = $FF5F
SETBNK          = $FF68
