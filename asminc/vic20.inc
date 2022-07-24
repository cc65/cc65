;
; VIC-20 generic definitions. Stolen mostly from c64.inc -- Steve Schmidtke
;

; ---------------------------------------------------------------------------
; Zero page, Commodore stuff

VARTAB          := $2D          ; Pointer to start of BASIC variables
MEMSIZE         := $37          ; Pointer to highest BASIC RAM location (+1)
TXTPTR          := $7A          ; Pointer into BASIC source code
STATUS          := $90          ; Kernal I/O completion status
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

RSSTAT          := $297         ; RS-232 device driver status

; ---------------------------------------------------------------------------
; Screen size

XSIZE           = 22
YSIZE           = 23

; ---------------------------------------------------------------------------
; Vector and other locations

IRQVec          := $0314
BRKVec          := $0316
NMIVec          := $0318

; ---------------------------------------------------------------------------
; I/O: 6560 VIC

VIC             := $9000
VIC_CR0         := VIC+$0
VIC_CR1         := VIC+$1
VIC_CR2         := VIC+$2
VIC_CR3         := VIC+$3
VIC_LINES       := VIC+$3       ; Screen lines, bit 7 is bit 0 from VIC_HLINE
VIC_CR4         := VIC+$4
VIC_HLINE       := VIC+$4       ; Rasterline, bits 1-8
VIC_CR5         := VIC+$5
VIC_CR6         := VIC+$6
VIC_CR7         := VIC+$7
VIC_CR8         := VIC+$8
VIC_CR9         := VIC+$9
VIC_CRA         := VIC+$A
VIC_CRB         := VIC+$B
VIC_CRC         := VIC+$C
VIC_CRD         := VIC+$D
VIC_CRE         := VIC+$E
VIC_CRF         := VIC+$F
VIC_COLOR       := VIC+$F       ; Border and background color

; ---------------------------------------------------------------------------
; I/O: 6522 VIA1

VIA1            := $9110        ; VIA1 base address
VIA1_JOY        := VIA1+$0      ; *** Deprecated ***
VIA1_PB         := VIA1+$0      ; Port register B
VIA1_PA1        := VIA1+$1      ; Port register A
VIA1_DDRB       := VIA1+$2      ; Data direction register B
VIA1_DDRA       := VIA1+$3      ; Data direction register A
VIA1_T1CL       := VIA1+$4      ; Timer 1, low byte
VIA1_T1CH       := VIA1+$5      ; Timer 1, high byte
VIA1_T1LL       := VIA1+$6      ; Timer 1 latch, low byte
VIA1_T1LH       := VIA1+$7      ; Timer 1 latch, high byte
VIA1_T2CL       := VIA1+$8      ; Timer 2, low byte
VIA1_T2CH       := VIA1+$9      ; Timer 2, high byte
VIA1_SR         := VIA1+$A      ; Shift register
VIA1_ACR        := VIA1+$B      ; Auxiliary control register
VIA1_PCR        := VIA1+$C      ; Peripheral control register
VIA1_IFR        := VIA1+$D      ; Interrupt flag register
VIA1_IER        := VIA1+$E      ; Interrupt enable register
VIA1_PA2        := VIA1+$F      ; Port register A w/o handshake

; ---------------------------------------------------------------------------
; I/O: 6522 VIA2

VIA2            := $9120        ; VIA2 base address
VIA2_JOY        := VIA2+$0      ; *** Deprecated ***
VIA2_PB         := VIA2+$0      ; Port register B
VIA2_PA1        := VIA2+$1      ; Port register A
VIA2_DDRB       := VIA2+$2      ; Data direction register B
VIA2_DDRA       := VIA2+$3      ; Data direction register A
VIA2_T1CL       := VIA2+$4      ; Timer 1, low byte
VIA2_T1CH       := VIA2+$5      ; Timer 1, high byte
VIA2_T1LL       := VIA2+$6      ; Timer 1 latch, low byte
VIA2_T1LH       := VIA2+$7      ; Timer 1 latch, high byte
VIA2_T2CL       := VIA2+$8      ; Timer 2, low byte
VIA2_T2CH       := VIA2+$9      ; Timer 2, high byte
VIA2_SR         := VIA2+$A      ; Shift register
VIA2_ACR        := VIA2+$B      ; Auxiliary control register
VIA2_PCR        := VIA2+$C      ; Peripheral control register
VIA2_IFR        := VIA2+$D      ; Interrupt flag register
VIA2_IER        := VIA2+$E      ; Interrupt enable register
VIA2_PA2        := VIA2+$F      ; Port register A w/o handshake
