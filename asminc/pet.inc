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

; 80-Column CBMs
KBDREPEAT80     := $E4
KBDRPTRATE80    := $E5
KBDRPTDELAY80   := $E6

BASIC_BUF       := $200         ; Location of command-line
BASIC_BUF_LEN   = 81            ; Maximum length of command-line

KEY_BUF         := $26F         ; Keyboard buffer

; 40-Column PETs/CBMs
KBDRPTDELAY40   := $3E9
KBDRPTRATE40    := $3EA
KBDREPEAT40     := $3EE
KBDREPEAT40B    := $3F8

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

VIA             := $E840        ; VIA base address
VIA_PB          := VIA+$0       ; Port register B
VIA_PA1         := VIA+$1       ; Port register A
VIA_PRB         := VIA+$0       ; *** Deprecated ***
VIA_PRA         := VIA+$1       ; *** Deprecated ***
VIA_DDRB        := VIA+$2       ; Data direction register B
VIA_DDRA        := VIA+$3       ; Data direction register A
VIA_T1CL        := VIA+$4       ; Timer 1, low byte
VIA_T1CH        := VIA+$5       ; Timer 1, high byte
VIA_T1LL        := VIA+$6       ; Timer 1 latch, low byte
VIA_T1LH        := VIA+$7       ; Timer 1 latch, high byte
VIA_T2CL        := VIA+$8       ; Timer 2, low byte
VIA_T2CH        := VIA+$9       ; Timer 2, high byte
VIA_SR          := VIA+$A       ; Shift register
VIA_CR          := VIA+$B       ; Auxiliary control register
VIA_PCR         := VIA+$C       ; Peripheral control register
VIA_IFR         := VIA+$D       ; Interrupt flag register
VIA_IER         := VIA+$E       ; Interrupt enable register
VIA_PA2         := VIA+$F       ; Port register A w/o handshake
