;
; Include file for the Commdore 6502 machines
;


;-----------------------------------------------------------------------------
; Subroutines available in the CBM jump table
;

.import C64MODE
.import SWAPPER
.import SETBNK

.import CINT
.import IOINIT
.import RAMTAS
.import VECTOR
.import RESTOR
.import SETMSG
.import SECOND
.import TKSA
.import MEMTOP
.import MEMBOT
.import SCNKEY
.import SETTMO
.import ACPTR
.import CIOUT
.import UNTLK
.import UNLSN
.import LISTEN
.import TALK
.import READST
.import SETLFS
.import SETNAM
.import OPEN
.import CLOSE
.import LOAD
.import SAVE
.import SETTIM
.import RDTIM
.import SCREEN
.import PLOT
.import IOBASE
.import CHKIN
.import CKOUT
.import CLRCH
.import BASIN
.import CHRIN
.import BSOUT
.import CHROUT
.import STOP
.import GETIN
.import CLALL
.import UDTIM

;-----------------------------------------------------------------------------
; Device numbers
;

CBMDEV_KBD      = 0
CBMDEV_DATASETTE= 1
CBMDEV_RS232    = 2
CBMDEV_SCREEN   = 3

;-----------------------------------------------------------------------------
; Disk stuff
;

MAX_DRIVES      = 23
FIRST_DRIVE     = 8
