;
; Ullrich von Bassewitz, 19.11.2002
;
; C128 kernal functions
;

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

;
CINT            = $FF81
IOINIT          = $FF84
RAMTAS          = $FF87
RESTOR          = $FF8A
VECTOR          = $FF8D
SETMSG          = $FF90
SECOND          = $FF93
TKSA            = $FF96
MEMTOP          = $FF99
MEMBOT          = $FF9C
SCNKEY          = $FF9F
SETTMO          = $FFA2
ACPTR           = $FFA5
CIOUT           = $FFA8
UNTLK           = $FFAB
UNLSN           = $FFAE
LISTEN          = $FFB1
TALK            = $FFB4
READST          = $FFB7
SETLFS          = $FFBA
SETNAM          = $FFBD
OPEN            = $FFC0
CLOSE           = $FFC3
CHKIN           = $FFC6
CKOUT           = $FFC9
CLRCH           = $FFCC
BASIN           = $FFCF
BSOUT           = $FFD2
LOAD            = $FFD5
SAVE            = $FFD8
SETTIM          = $FFDB
RDTIM           = $FFDE
STOP            = $FFE1
GETIN           = $FFE4
CLALL           = $FFE7
UDTIM           = $FFEA
SCREEN          = $FFED
PLOT            = $FFF0
IOBASE          = $FFF3

