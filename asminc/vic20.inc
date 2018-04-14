;
; Vic20 generic definitions. Stolen mostly from c64.inc - Steve Schmidtke
;


; ---------------------------------------------------------------------------
; Zero page, Commodore stuff

VARTAB          := $2D          ; Pointer to start of BASIC variables
MEMSIZE         := $37          ; Pointer to highest BASIC RAM location (+1)
TXTPTR          := $7A          ; Pointer into BASIC source code
TIME            := $A0          ; 60HZ clock
FNAM_LEN        := $B7          ; Length of filename
SECADR          := $B9          ; Secondary address
DEVNUM          := $BA          ; Device number
FNAM            := $BB          ; Pointer to filename
KEY_COUNT       := $C6          ; Number of keys in input buffer
RVS             := $C7          ; Reverse flag
CURS_FLAG       := $CC          ; 1 = cursor off
CURS_BLINK      := $CD          ; Blink counter
CURS_CHAR       := $CE          ; Character under the cursor
CURS_STATE      := $CF          ; Cursor blink state
SCREEN_PTR      := $D1          ; Pointer to current char in text screen
CURS_X          := $D3          ; Cursor column
CURS_Y          := $D6          ; Cursor row
CRAM_PTR        := $F3          ; Pointer to current char in color RAM

BASIC_BUF       := $200         ; Location of command-line
BASIC_BUF_LEN   = 89            ; Maximum length of command-line

CHARCOLOR       := $286
CURS_COLOR      := $287         ; Color under the cursor

KBDREPEAT       := $28a
KBDREPEATRATE   := $28b
KBDREPEATDELAY  := $28c

; ---------------------------------------------------------------------------
; Screen size

XSIZE           = 22
YSIZE           = 23

; ---------------------------------------------------------------------------
; Kernal routines

; Direct entries
CLRSCR          := $E55F
KBDREAD         := $E5CF

; ---------------------------------------------------------------------------
; Vector and other locations

IRQVec          := $0314
BRKVec          := $0316
NMIVec          := $0318

; ---------------------------------------------------------------------------
; I/O: 6560 VIC

VIC             := $9000
VIC_LINES       := $9003        ; Screen lines, bit 7 is bit 0 from VIC_HLINE
VIC_HLINE       := $9004        ; Rasterline, bits 1-8
VIC_COLOR       := $900F        ; Border and background color

; ---------------------------------------------------------------------------
; I/O: 6522 VIA1

VIA1            := $9110        ; VIA1 base address
VIA1_JOY        := $9111        ; *** Deprecated ***
VIA1_PB         := $9110        ; Port register B
VIA1_PA1        := $9111        ; Port register A
VIA1_DDRB       := $9112        ; Data direction register B
VIA1_DDRA       := $9113        ; Data direction register A
VIA1_T1CL       := $9114        ; Timer 1, low byte
VIA1_T1CH       := $9115        ; Timer 1, high byte
VIA1_T1LL       := $9116        ; Timer 1 latch, low byte
VIA1_T1LH       := $9117        ; Timer 1 latch, high byte
VIA1_T2CL       := $9118        ; Timer 2, low byte
VIA1_T2CH       := $9119        ; Timer 2, high byte
VIA1_SR         := $911A        ; Shift register
VIA1_CR         := $911B        ; Auxiliary control register
VIA1_PCR        := $911C        ; Peripheral control register
VIA1_IFR        := $911D        ; Interrupt flag register
VIA1_IER        := $911E        ; Interrupt enable register
VIA1_PA2        := $911F        ; Port register A w/o handshake

; ---------------------------------------------------------------------------
; I/O: 6522 VIA2

VIA2            := $9120        ; VIA2 base address
VIA2_JOY        := $9120        ; *** Deprecated ***
VIA2_PB         := $9120        ; Port register B
VIA2_PA1        := $9121        ; Port register A
VIA2_DDRB       := $9122        ; Data direction register B
VIA2_DDRA       := $9123        ; Data direction register A
VIA2_T1CL       := $9124        ; Timer 1, low byte
VIA2_T1CH       := $9125        ; Timer 1, high byte
VIA2_T1LL       := $9126        ; Timer 1 latch, low byte
VIA2_T1LH       := $9127        ; Timer 1 latch, high byte
VIA2_T2CL       := $9128        ; Timer 2, low byte
VIA2_T2CH       := $9129        ; Timer 2, high byte
VIA2_SR         := $912A        ; Shift register
VIA2_CR         := $912B        ; Auxiliary control register
VIA2_PCR        := $912C        ; Peripheral control register
VIA2_IFR        := $912D        ; Interrupt flag register
VIA2_IER        := $912E        ; Interrupt enable register
VIA2_PA2        := $912F        ; Port register A w/o handshake
