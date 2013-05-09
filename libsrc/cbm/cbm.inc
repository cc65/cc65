;
; Include file for the Commdore 6502 machines
;


;-----------------------------------------------------------------------------
; Subroutines available in the CBM jump table
;

;CINT           = $FF81
;IOINIT         = $FF84
;RAMTAS         = $FF87
;RESTOR         = $FF8A
;VECTOR         = $FF8D
;SETMSG         = $FF90
;SECOND         = $FF93
;TKSA           = $FF96
;MEMTOP         = $FF99
;MEMBOT         = $FF9C
;SCNKEY         = $FF9F
;SETTMO         = $FFA2
;ACPTR          = $FFA5
;CIOUT          = $FFA8
;UNTLK          = $FFAB
;UNLSN          = $FFAE
;LISTEN         = $FFB1
;TALK           = $FFB4
;READST         = $FFB7
;SETLFS         = $FFBA
;SETNAM         = $FFBD
;OPEN           = $FFC0
;CLOSE          = $FFC3
;CHKIN          = $FFC6
;CKOUT          = $FFC9
;CLRCH          = $FFCC
;BASIN          = $FFCF
;BSOUT          = $FFD2
;LOAD           = $FFD5
;SAVE           = $FFD8
;SETTIM         = $FFDB
;RDTIM          = $FFDE
;STOP           = $FFE1
;GETIN          = $FFE4
;CLALL          = $FFE7
;UDTIM          = $FFEA
;SCREEN         = $FFED
;PLOT           = $FFF0
;IOBASE         = $FFF3


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


