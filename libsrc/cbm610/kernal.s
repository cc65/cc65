;
; Ullrich von Bassewitz, 2003-12-20
;
; CBM610 kernal functions
;

	.include "cbmkernal.inc"

        .export         CINT
        .export         IOINIT
;        .export         RAMTAS ; not available???
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
        .export         SETLFS
        .export         CHKIN
        .export         CKOUT
        .export         CLRCH
        .export         BASIN
        .export         BSOUT
        .export         LOAD
        .export         SAVE
        .export         STOP
        .export         GETIN
        .export         CLALL
        .export         PLOT
