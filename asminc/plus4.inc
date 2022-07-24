;
; Plus/4 generic definitions.
;


; ---------------------------------------------------------------------------
; Zero page, Commodore stuff

TMPPTR          := $22          ; Temporary ptr used by BASIC
VARTAB          := $2D          ; Pointer to start of BASIC variables
MEMSIZE         := $37          ; Pointer to highest BASIC RAM location (+1)
TXTPTR          := $3B          ; Pointer into BASIC source code
STATUS          := $90          ; Kernal I/O completion status
TIME            := $A3          ; 60HZ clock
FNAM_LEN        := $AB          ; Length of filename
LFN             := $AC          ; Logical file number
SECADR          := $AD          ; Secondary address
DEVNUM          := $AE          ; Device number
FNAM            := $AF          ; Pointer to filename for OPEN
KEY_COUNT       := $EF          ; Number of keys in input buffer
RVS             := $C2          ; Reverse flag
CURS_X          := $CA          ; Cursor column
CURS_Y          := $CD          ; Cursor row
SCREEN_PTR      := $C8          ; Pointer to current char in text screen
CRAM_PTR        := $EA          ; Pointer to current char in color RAM

BASIC_BUF       := $200         ; Location of command-line
BASIC_BUF_LEN   = 89            ; Maximum length of command-line

FNBUF           := $25E         ; Buffer for filename
FETCH           := $494         ; lda (zp),y from RAM
CHARCOLOR       := $53B
FKEY_COUNT      := $55D         ; Characters for function key
FKEY_SPACE      := $55F         ; Function key definitions
FKEY_ORIG       := $F3D2        ; Original definitions

KBDREPEAT       := $540
KBDREPEATRATE   := $541
KBDREPEATDELAY  := $542

; ---------------------------------------------------------------------------
; Vector and other locations

IRQVec          := $0314
BRKVec          := $0316
NMIVec          := $0318

; ---------------------------------------------------------------------------
; Screen size

XSIZE           = 40
YSIZE           = 25

; ---------------------------------------------------------------------------
; I/O

TED_T1LO        := $FF00
TED_T1HI        := $FF01
TED_T2LO        := $FF02
TED_T2HI        := $FF03
TED_T3LO        := $FF04
TED_T4HI        := $FF05
TED_MULTI1      := $FF07
TED_KBD         := $FF08
TED_CURSHI      := $FF0C
TED_CURSLO      := $FF0D
TED_V1FRQLO     := $FF0E
TED_V2FRQLO     := $FF0F
TED_V2FRQHI     := $FF10
TED_CLK         := $FF13
TED_BGCOLOR     := $FF15
TED_COLOR1      := $FF16
TED_COLOR2      := $FF17
TED_COLOR3      := $FF18
TED_BORDERCOLOR := $FF19
TED_VLINEHI     := $FF1C
TED_VLINELO     := $FF1D
TED_HPOS        := $FF1E
TED_ROMSEL      := $FF3E
TED_RAMSEL      := $FF3F

; ---------------------------------------------------------------------------
; RAM/ROM selection addresses

ENABLE_ROM      := TED_ROMSEL
ENABLE_RAM      := TED_RAMSEL
