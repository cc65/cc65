;
; Olli Savia <ops@iki.fi>
;
; Commodore Kernal functions
;

.if .def(__CX16__)
  ; CX16 extended jump table
  GETJOY       := $FF06
  MOUSE        := $FF09
  SCRMOD       := $FF5F
.endif

.if .def(__C128__)
  ; C128 extended jump table
  C64MODE      := $FF4D
  SWAPPER      := $FF5F
  SETBNK       := $FF68
.endif

.if .def(__C128__) || .def(__CX16__)
  ; Extended jump table
  CLSALL       := $FF4A
  JSRFAR       := $FF6E
  INDFET       := $FF74
  INDSTA       := $FF77
  INDCMP       := $FF7A
  PRIMM        := $FF7D
.endif

.if .def(__C64__) || .def(__C128__) || .def(__C16__) || .def(__CX16__)
  CINT         := $FF81
  IOINIT       := $FF84
  RAMTAS       := $FF87
.elseif .def(__VIC20__)
  CINT         := $E518         ; No entries are in the Kernal jump table of the VIC-20 for these three (3) functions.
  IOINIT       := $FDF9         ; The entries for these functions have been set to point directly to the functions
  RAMTAS       := $FD8D         ; in the Kernal, to maintain compatibility with the other Commodore platforms.
.elseif .def(__CBM510__) || .def(__CBM610__)
  IOINIT       := $FF7B
  CINT         := $FF7E
.endif

.if .def(__VIC20__) || .def(__C64__) || .def(__C128__) || .def(__C16__) || .def(__CX16__)
  RESTOR       := $FF8A
  VECTOR       := $FF8D
.elseif .def(__CBM510__) || .def(__CBM610__)
  VECTOR       := $FF84
  RESTOR       := $FF87
.endif

.if .def(__CBM510__) || .def(__CBM610__) || .def(__VIC20__) || .def(__C64__) || .def(__C128__) || .def(__C16__) || .def(__CX16__)
  SETMSG       := $FF90
  SECOND       := $FF93
  TKSA         := $FF96
  MEMTOP       := $FF99
  MEMBOT       := $FF9C
  SCNKEY       := $FF9F
  SETTMO       := $FFA2
  ACPTR        := $FFA5
  CIOUT        := $FFA8
  UNTLK        := $FFAB
  UNLSN        := $FFAE
  LISTEN       := $FFB1
  TALK         := $FFB4
  READST       := $FFB7
  SETLFS       := $FFBA
  SETNAM       := $FFBD
  OPEN         := $FFC0
  CLOSE        := $FFC3
.endif

; Available on all platforms including PET
CHKIN          := $FFC6
CKOUT          := $FFC9
CHKOUT         := $FFC9
CLRCH          := $FFCC
CLRCHN         := $FFCC
BASIN          := $FFCF
CHRIN          := $FFCF
BSOUT          := $FFD2
CHROUT         := $FFD2

.if .def(__CBM510__) || .def(__CBM610__) || .def(__VIC20__) || .def(__C64__) || .def(__C128__) || .def(__C16__) || .def(__CX16__)
  LOAD         := $FFD5
  SAVE         := $FFD8
  SETTIM       := $FFDB
  RDTIM        := $FFDE
.endif

; Available on all platforms including PET
STOP           := $FFE1
GETIN          := $FFE4
CLALL          := $FFE7
UDTIM          := $FFEA

.if .def(__CBM510__) || .def(__CBM610__) || .def(__VIC20__) || .def(__C64__) || .def(__C128__) || .def(__C16__) || .def(__CX16__)
  SCREEN       := $FFED
  PLOT         := $FFF0
  IOBASE       := $FFF3
.endif

; ---------------------------------------------------------------------------
; Kernal routines, direct entries
;
; Unlike the above, these are not standard functions with entries in the jump
; table. They do not exist in all Kernals, and where they do the entry point is
; specific to that particular machine and possibly even Kernal version.
;
; This list is not comprehensive: missing items for particular machines
; should be added as needed.
;
; UPDCRAMPTR: Updates the color RAM pointer to match the screen RAM pointer.
;

.if .def(__VIC20__)
  CLRSCR       := $E55F
  KBDREAD      := $E5CF
  UPDCRAMPTR   := $EAB2
.elseif .def(__C64__)
  CLRSCR       := $E544
  KBDREAD      := $E5B4
  NMIEXIT      := $FEBC
  UPDCRAMPTR   := $EA24
.elseif .def(__C128__)
  CLRSCR       := $C142
  KBDREAD      := $C006
  NMIEXIT      := $FF33
  NEWLINE      := $C363
  PRINT        := $C322
  CURS_SET     := $CD57
  CURS_ON      := $CD6F
  CURS_OFF     := $CD9F
.elseif .def(__C16__)
  CLRSCR       := $D88B
  KBDREAD      := $D8C1
.endif
