;
; PET generic definitions.
;


; ---------------------------------------------------------------------------
; Zero page, Commodore stuff

VARTAB          := $2A          ; Pointer to start of BASIC variables
MEMSIZE         := $34          ; Size of memory installed
TXTPTR          := $77          ; Pointer into BASIC source code
TIME            := $8D          ; 60HZ clock
KEY_COUNT       := $9E          ; Number of keys in input buffer
RVS             := $9F          ; Reverse flag
CURS_FLAG       := $A7          ; 1 = cursor off
CURS_BLINK      := $A8          ; Blink counter
CURS_CHAR       := $A9          ; Character under the cursor
CURS_STATE      := $AA          ; Cursor blink state
SCREEN_PTR      := $C4          ; Pointer to current char in text screen
CURS_X          := $C6          ; Cursor column
FNLEN           := $D1          ; Length of filename
LFN             := $D2          ; Current Logical File Number
SECADR          := $D3          ; Secondary address
DEVNUM          := $D4          ; Device number
SCR_LINELEN     := $D5          ; Screen line length
CURS_Y          := $D8          ; Cursor row
FNADR           := $DA          ; Pointer to file name

BASIC_BUF       := $200         ; Location of command-line
BASIC_BUF_LEN   = 81            ; Maximum length of command-line

KEY_BUF         := $26F         ; Keyboard buffer

;----------------------------------------------------------------------------
; PET ROM type detection

PET_DETECT      := $FFFB
PET_2000        = $CA
PET_3000        = $FC
PET_4000        = $FD


;----------------------------------------------------------------------------
; Vector and other locations

IRQVec          := $0090
BRKVec          := $0092
NMIVec          := $0094

; ---------------------------------------------------------------------------
; I/O: 6522 VIA2

VIA             := $E840
VIA_PRB         := $E840
VIA_PRA         := $E841
VIA_DDRB        := $E842
VIA_DDRA        := $E843


