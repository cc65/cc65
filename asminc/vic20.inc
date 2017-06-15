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

VIA1            := $9110
VIA1_JOY        := $9111
VIA1_DDRB       := $9112
VIA1_DDRA       := $9113

; ---------------------------------------------------------------------------
; I/O: 6522 VIA2

VIA2            := $9120
VIA2_JOY        := $9120
VIA2_DDRB       := $9122
VIA2_DDRA       := $9123

