;
; Olli Savia <ops@iki.fi>
;
; Commodore kernal functions
;

.if .def(__C128__)
  ; C128 Extended jump table
  C64MODE      := $FF4D
  SWAPPER      := $FF5F
  SETBNK       := $FF68
.endif

.if .def(__C64__) || .def(__C128__) || .def(__C16__)
  CINT         := $FF81
  IOINIT       := $FF84
  RAMTAS       := $FF87
.elseif .def(__VIC20__)
  CINT         := $E518         ; No entries are in the kernal jump table of the Vic20 for these three (3) functions.
  IOINIT       := $FDF9         ; The entries for these functions have been set to point directly to the functions
  RAMTAS       := $FD8D         ; in the kernal to maintain compatibility with the other Commodore platforms.
.elseif .def(__CBM510__) || .def(__CBM610__)
  IOINIT       := $FF7B
  CINT         := $FF7E
.endif

.if .def(__VIC20__) || .def(__C64__) || .def(__C128__) || .def(__C16__)
  RESTOR       := $FF8A
  VECTOR       := $FF8D
.elseif .def(__CBM510__) || .def(__CBM610__)
  VECTOR       := $FF84
  RESTOR       := $FF87
.endif

.if .def(__CBM510__) || .def(__CBM610__) || .def(__VIC20__) || .def(__C64__) || .def(__C128__) || .def(__C16__)
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

.if .def(__CBM510__) || .def(__CBM610__) || .def(__VIC20__) || .def(__C64__) || .def(__C128__) || .def(__C16__)
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

.if .def(__CBM510__) || .def(__CBM610__) || .def(__VIC20__) || .def(__C64__) || .def(__C128__) || .def(__C16__)
  SCREEN       := $FFED
  PLOT         := $FFF0
  IOBASE       := $FFF3
.endif

; ---------------------------------------------------------------------------
; Kernal routines, direct entries

.if .def(__VIC20__)
  CLRSCR       := $E55F
  KBDREAD      := $E5CF
.elseif .def(__C64__)
  CLRSCR       := $E544
  KBDREAD      := $E5B4
  NMIEXIT      := $FEBC
.elseif .def(__C128__)
  CLRSCR       := $C142
  KBDREAD      := $C006
  NMIEXIT      := $FF33
  NEWLINE      := $C363
  PRINT        := $C322
  CURS_SET     := $CD57
  CURS_ON      := $CD6F
  CURS_OFF     := $CD9F
  INDFET       := $FF74
.elseif .def(__C16__)
  CLRSCR       := $D88B
  KBDREAD      := $D8C1
.endif
